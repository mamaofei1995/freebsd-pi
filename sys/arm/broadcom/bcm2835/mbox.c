/*-
 * Copyright (c) 2012 Oleksandr Tymoshenko <gonzo@freebsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/malloc.h>
#include <sys/rman.h>
#include <sys/timeet.h>
#include <sys/timetc.h>
#include <sys/watchdog.h>
#include <machine/bus.h>
#include <machine/cpu.h>
#include <machine/frame.h>
#include <machine/intr.h>

#include <dev/fdt/fdt_common.h>
#include <dev/ofw/openfirm.h>
#include <dev/ofw/ofw_bus.h>
#include <dev/ofw/ofw_bus_subr.h>

#include <machine/bus.h>
#include <machine/fdt.h>
#include <arm/broadcom/bcm2835/mbox.h>

#define	REG_READ	0x00
#define	REG_POL		0x10
#define	REG_SENDER	0x14
#define	REG_STATUS	0x18
#define		STATUS_FULL	0x80000000
#define		STATUS_EMPTY	0x40000000
#define	REG_CONFIG	0x1C
#define		CONFIG_DATA_IRQ	0x00000001
#define	REG_WRITE	0x20 /* This is Mailbox 1 address */

#define	MBOX_MSG(chan, data)	(((data) & ~0xf) | ((chan) & 0xf))
#define	MBOX_CHAN(msg)		((msg) & 0xf)
#define	MBOX_DATA(msg)		((msg) & ~0xf)

#define	MBOX_LOCK	do {		\
	mtx_lock(&brcm_mbox_sc->lock);	\
} while(0)

#define	MBOX_UNLOCK	do {		\
	mtx_unlock(&brcm_mbox_sc->lock);	\
} while(0)

struct brcm_mbox_softc {
	struct mtx		lock;
	struct resource *	mem_res;
	struct resource *	irq_res;
	void*			intr_hl;
	bus_space_tag_t		bst;
	bus_space_handle_t	bsh;
	int			valid[BCM2835_MBOX_CHANS];
	int			msg[BCM2835_MBOX_CHANS];
};

static struct brcm_mbox_softc *brcm_mbox_sc = NULL;

#define	mbox_read_4(reg)		\
    bus_space_read_4(brcm_mbox_sc->bst, brcm_mbox_sc->bsh, reg)
#define	mbox_write_4(reg, val)		\
    bus_space_write_4(brcm_mbox_sc->bst, brcm_mbox_sc->bsh, reg, val)

static void
brcm_mbox_intr(void *arg)
{
	struct brcm_mbox_softc *sc = arg;
	int chan;
	uint32_t data;
	uint32_t msg;

	MBOX_LOCK;
	while (!(mbox_read_4(REG_STATUS) & STATUS_EMPTY)) {
		msg = mbox_read_4(REG_READ);
		chan = MBOX_CHAN(msg);
		data = MBOX_DATA(msg);
		if (sc->valid[chan]) {
			printf("brcm_mbox_intr: channel %d oveflow\n", chan);
			continue;
		}
		sc->msg[chan] = data;
		sc->valid[chan] = 1;
		wakeup(&sc->msg[chan]);
		
	}
	MBOX_UNLOCK;
}

static int
brcm_mbox_probe(device_t dev)
{

	if (ofw_bus_is_compatible(dev, "broadcom,bcm2835-mbox")) {
		device_set_desc(dev, "BCM2835 VideoCore Mailbox");
		return(BUS_PROBE_DEFAULT);
	}

	return (ENXIO);
}

static int
brcm_mbox_attach(device_t dev)
{
	struct brcm_mbox_softc *sc = device_get_softc(dev);
	int i;
	int rid = 0;

	if (brcm_mbox_sc != NULL)
		return (EINVAL);

	sc->mem_res = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &rid, RF_ACTIVE);
	if (sc->mem_res == NULL) {
		device_printf(dev, "could not allocate memory resource\n");
		return (ENXIO);
	}

	sc->bst = rman_get_bustag(sc->mem_res);
	sc->bsh = rman_get_bushandle(sc->mem_res);

	rid = 0;
	sc->irq_res = bus_alloc_resource_any(dev, SYS_RES_IRQ, &rid, RF_ACTIVE);
	if (sc->irq_res == NULL) {
		device_printf(dev, "could not allocate interrupt resource\n");
		return (ENXIO);
	}

	/* Setup and enable the timer */
	if (bus_setup_intr(dev, sc->irq_res, INTR_TYPE_MISC,
			NULL, brcm_mbox_intr, sc,
			&sc->intr_hl) != 0) {
		bus_release_resource(dev, SYS_RES_IRQ, rid,
			sc->irq_res);
		device_printf(dev, "Unable to setup the clock irq handler.\n");
		return (ENXIO);
	}

	mtx_init(&sc->lock, "vcio mbox", MTX_DEF, 0);
	for (i = 0; i < BCM2835_MBOX_CHANS; i++) {
		sc->valid[0] = 0;
		sc->msg[0] = 0;
	}

	brcm_mbox_sc = sc;

	/* Should be called after brcm_mbox_sc initialization */
	mbox_write_4(REG_CONFIG, CONFIG_DATA_IRQ);

	return (0);
}

static device_method_t brcm_mbox_methods[] = {
	DEVMETHOD(device_probe,		brcm_mbox_probe),
	DEVMETHOD(device_attach,	brcm_mbox_attach),
	{ 0, 0 }
};

static driver_t brcm_mbox_driver = {
	"mbox",
	brcm_mbox_methods,
	sizeof(struct brcm_mbox_softc),
};

static devclass_t brcm_mbox_devclass;

DRIVER_MODULE(mbox, simplebus, brcm_mbox_driver, brcm_mbox_devclass, 0, 0);

/* 
 * Mailbox API
 */

int
brcm_mbox_write(int chan, uint32_t data)
{
	int limit = 20000;

	MBOX_LOCK;

	while ((mbox_read_4(REG_STATUS) & STATUS_FULL) && limit--) {
		DELAY(2);
	}

	if (limit == 0) {
		printf("brcm_mbox_write: STATUS_FULL stuck");
		MBOX_UNLOCK;
		return (EAGAIN);
	}
	
	mbox_write_4(REG_WRITE, MBOX_MSG(chan, data));

	MBOX_UNLOCK;
	return (0);
}

int
brcm_mbox_read(int chan, uint32_t *data)
{
	struct brcm_mbox_softc *sc = brcm_mbox_sc;

	MBOX_LOCK;
	while (!sc->valid[chan])
		msleep(&sc->msg[chan], &sc->lock, PZERO, "vcio mbox read", 0);
	*data = MBOX_DATA(brcm_mbox_sc->msg[chan]);
	brcm_mbox_sc->valid[chan] = 0;
	MBOX_UNLOCK;

	return (0);
}
