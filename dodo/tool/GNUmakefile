###############################################################################
#
# $Id: GNUmakefile,v 1.10 2009/06/01 11:46:51 stuart_hc Exp $ $Name:  $
#
# The contents of this file are subject to the AAF SDK Public Source
# License Agreement Version 2.0 (the "License"); You may not use this
# file except in compliance with the License.  The License is available
# in AAFSDKPSL.TXT, or you may obtain a copy of the License from the
# Advanced Media Workflow Association, Inc., or its successor.
#
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
# the License for the specific language governing rights and limitations
# under the License.  Refer to Section 3.3 of the License for proper use
# of this Exhibit.
#
# WARNING:  Please contact the Advanced Media Workflow Association,
# Inc., for more information about any additional licenses to
# intellectual property covering the AAF Standard that may be required
# to create and distribute AAF compliant products.
# (http://www.amwa.tv/policies).
#
# Copyright Notices:
# The Original Code of this file is Copyright 1998-2009, licensor of the
# Advanced Media Workflow Association.  All rights reserved.
#
# The Initial Developer of the Original Code of this file and the
# licensor of the Advanced Media Workflow Association is
# Avid Technology.
# All rights reserved.
#
###############################################################################

PACKAGE = dodoTool

AAFBASE ?= ../..


# Common definitions
include $(AAFBASE)/build/common.mk


INCLUDES = -I$(AAFBASE)/ref-impl/bld_cfg


# DODO_COMMON_OBJS is the list of object files used both in 
# dodo executable and dodo test application.
#
DODO_COMMON_OBJS = $(OBJDIR)/SourceInfo$(OBJ) $(OBJDIR)/TextStream$(OBJ) \
	$(OBJDIR)/ArgDef$(OBJ) $(OBJDIR)/ArgSet$(OBJ) \
	$(OBJDIR)/MacroDef$(OBJ) $(OBJDIR)/MacroSet$(OBJ)

# DODO_APP_OBJS is the list of object files used in dodo tool.
#
DODO_APP_OBJS = $(DODO_COMMON_OBJS) $(OBJDIR)/DodoMain$(OBJ)

# DODO_TEST_OBJS is the list of object files used in dodo test application.
#
DODO_TEST_OBJS = $(DODO_COMMON_OBJS) $(OBJDIR)/TestMain$(OBJ) \
	$(OBJDIR)/SourceInfoTest$(OBJ) $(OBJDIR)/TextStreamTest$(OBJ) \
	$(OBJDIR)/ArgDefTest$(OBJ) $(OBJDIR)/ArgSetTest$(OBJ) \
	$(OBJDIR)/MacroDefTest$(OBJ) $(OBJDIR)/MacroSetTest$(OBJ)


.PHONY : all
all : dodo
#all : dodo test


.PHONY : dodo
dodo: $(OBJDIR)/dodo$(EXE)


.PHONY : test
test: $(OBJDIR)/dodo.test$(EXE)


# Note: Although all the object files are in dependencies list
# we don't link all of them to make the target. 
# 
$(OBJDIR)/dodo$(EXE): $(OBJDIR) $(CXXOBJS)
	$(CC) $(CFLAGS) $(DODO_APP_OBJS) -o $@


# Note: Although all the object files are in dependencies list
# we don't link all of them to make the target. 
# 
$(OBJDIR)/dodo.test$(EXE) : $(OBJDIR) $(CXXOBJS)
	$(CC) $(CFLAGS) $(DODO_TEST_OBJS) -o $@


.PHONY : clean
clean :
	@$(RM) $(DODO_APP_OBJS) $(DODO_TEST_OBJS)
	@$(RM) $(DEPS)
	@$(RM) $(OBJDIR)/dodo$(EXE)
	@$(RM) $(OBJDIR)/dodo.test$(EXE)


.PHONY : realclean
realclean : clean


include $(AAFBASE)/build/rules.mk

