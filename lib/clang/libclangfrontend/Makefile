# $FreeBSD: projects/armv6/lib/clang/libclangfrontend/Makefile 234858 2012-05-01 04:01:22Z gonzo $

LIB=	clangfrontend

SRCDIR=	tools/clang/lib/Frontend
SRCS=	ASTConsumers.cpp \
	ASTMerge.cpp \
	ASTUnit.cpp \
	CacheTokens.cpp \
	ChainedDiagnosticConsumer.cpp \
	ChainedIncludesSource.cpp \
	CompilerInstance.cpp \
	CompilerInvocation.cpp \
	CreateInvocationFromCommandLine.cpp \
	DependencyFile.cpp \
	DependencyGraph.cpp \
	DiagnosticRenderer.cpp \
	FrontendAction.cpp \
	FrontendActions.cpp \
	FrontendOptions.cpp \
	HeaderIncludeGen.cpp \
	InitHeaderSearch.cpp \
	InitPreprocessor.cpp \
	LangStandards.cpp \
	LayoutOverrideSource.cpp \
	LogDiagnosticPrinter.cpp \
	MultiplexConsumer.cpp \
	PrintPreprocessedOutput.cpp \
	SerializedDiagnosticPrinter.cpp \
	TextDiagnostic.cpp \
	TextDiagnosticBuffer.cpp \
	TextDiagnosticPrinter.cpp \
	VerifyDiagnosticConsumer.cpp \
	Warnings.cpp

TGHDRS=	AttrList \
	AttrParsedAttrList \
	Attrs \
	CC1Options \
	DeclNodes \
	DiagnosticASTKinds \
	DiagnosticCommonKinds \
	DiagnosticDriverKinds \
	DiagnosticFrontendKinds \
	DiagnosticLexKinds \
	DiagnosticSemaKinds \
	Options \
	StmtNodes

.include "../clang.lib.mk"
