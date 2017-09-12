TEMPLATE = subdirs

CONFIG += ordered

# The order here is important!
SUBDIRS += \
    acknext \  # Engine is first
    addons \   # Then comes the list of addons
    tools \    # After that, the tools are built
	prototypes # And finally, the prototypes

HEADERS += \
    documentation/src/groups.h \
    documentation/src/mainpage.h

