# GNU Make project makefile autogenerated by Premake
ifndef config
  config=debug64
endif

ifndef verbose
  SILENT = @
endif

CC = gcc
CXX = g++
AR = ar

ifndef RESCOMP
  ifdef WINDRES
    RESCOMP = $(WINDRES)
  else
    RESCOMP = windres
  endif
endif

ifeq ($(config),debug64)
  OBJDIR     = obj/x64/Debug
  TARGETDIR  = ../../../bin
  TARGET     = $(TARGETDIR)/tinymicrod
  DEFINES   += -D__orxDEBUG__
  INCLUDES  += -I$(ORX)/include -I../../../include/Scroll -I../../../include
  ALL_CPPFLAGS  += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS    += $(CFLAGS) $(ALL_CPPFLAGS) -ffast-math -g -m64 -stdlib=libc++ -gdwarf-2 -Wno-unused-function -Wno-write-strings -std=c++20
  ALL_CXXFLAGS  += $(CXXFLAGS) $(ALL_CFLAGS)
  ALL_RESFLAGS  += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  ALL_LDFLAGS   += $(LDFLAGS) -L$(ORX)/lib/dynamic -L. -m64 -L/usr/lib64 -stdlib=libc++ -dead_strip
  LIBS      += -lorxd -framework Foundation -framework AppKit
  LDDEPS    +=
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
	@echo Running post-build commands
	cp -f $(ORX)/lib/dynamic/liborx*.dylib ../../../bin
  endef
endif

ifeq ($(config),profile64)
  OBJDIR     = obj/x64/Profile
  TARGETDIR  = ../../../bin
  TARGET     = $(TARGETDIR)/tinymicrop
  DEFINES   += -D__orxPROFILER__
  INCLUDES  += -I$(ORX)/include -I../../../include/Scroll -I../../../include
  ALL_CPPFLAGS  += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS    += $(CFLAGS) $(ALL_CPPFLAGS) -ffast-math -g -O2 -m64 -stdlib=libc++ -gdwarf-2 -Wno-unused-function -Wno-write-strings -std=c++20
  ALL_CXXFLAGS  += $(CXXFLAGS) $(ALL_CFLAGS)
  ALL_RESFLAGS  += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  ALL_LDFLAGS   += $(LDFLAGS) -L$(ORX)/lib/dynamic -L. -m64 -L/usr/lib64 -stdlib=libc++ -dead_strip
  LIBS      += -lorxp -framework Foundation -framework AppKit
  LDDEPS    +=
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
	@echo Running post-build commands
	cp -f $(ORX)/lib/dynamic/liborx*.dylib ../../../bin
  endef
endif

ifeq ($(config),release64)
  OBJDIR     = obj/x64/Release
  TARGETDIR  = ../../../bin
  TARGET     = $(TARGETDIR)/tinymicro
  DEFINES   +=
  INCLUDES  += -I$(ORX)/include -I../../../include/Scroll -I../../../include
  ALL_CPPFLAGS  += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
  ALL_CFLAGS    += $(CFLAGS) $(ALL_CPPFLAGS) -ffast-math -g -O2 -m64 -stdlib=libc++ -gdwarf-2 -Wno-unused-function -Wno-write-strings -std=c++20
  ALL_CXXFLAGS  += $(CXXFLAGS) $(ALL_CFLAGS)
  ALL_RESFLAGS  += $(RESFLAGS) $(DEFINES) $(INCLUDES)
  ALL_LDFLAGS   += $(LDFLAGS) -L$(ORX)/lib/dynamic -L. -m64 -L/usr/lib64 -stdlib=libc++ -dead_strip
  LIBS      += -lorx -framework Foundation -framework AppKit
  LDDEPS    +=
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
	@echo Running post-build commands
	cp -f $(ORX)/lib/dynamic/liborx*.dylib ../../../bin
  endef
endif

OBJECTS := \
	$(OBJDIR)/LDtkWorld.o \
	$(OBJDIR)/Character.o \
	$(OBJDIR)/orxLDtk.o \
	$(OBJDIR)/Score.o \
	$(OBJDIR)/HealthBar.o \
	$(OBJDIR)/Object.o \
	$(OBJDIR)/Mob.o \
	$(OBJDIR)/tinymicro.o \
	$(OBJDIR)/GameMenu.o \

RESOURCES := \

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

.PHONY: clean prebuild prelink

all: $(TARGETDIR) $(OBJDIR) prebuild prelink $(TARGET)
	@:

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking tinymicro
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning tinymicro
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
.NOTPARALLEL: $(GCH) $(PCH)
$(GCH): $(PCH)
	@echo $(notdir $<)
	$(SILENT) $(CXX) -x c++-header $(ALL_CXXFLAGS) -MMD -MP $(DEFINES) $(INCLUDES) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
endif

$(OBJDIR)/LDtkWorld.o: ../../../src/LDtkWorld.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

$(OBJDIR)/Character.o: ../../../src/Character.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

$(OBJDIR)/orxLDtk.o: ../../../src/orxLDtk.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

$(OBJDIR)/Score.o: ../../../src/Score.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

$(OBJDIR)/HealthBar.o: ../../../src/HealthBar.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

$(OBJDIR)/Object.o: ../../../src/Object.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

$(OBJDIR)/Mob.o: ../../../src/Mob.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

$(OBJDIR)/tinymicro.o: ../../../src/tinymicro.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

$(OBJDIR)/GameMenu.o: ../../../src/GameMenu.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(OBJDIR)/$(notdir $(PCH)).d
endif
