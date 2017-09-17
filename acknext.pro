TEMPLATE = subdirs

CONFIG += ordered

# The order here is important!
SUBDIRS += \
    acknext \  # Engine is first
    addons \   # Then comes the list of addons
    tools \    # After that, the tools are built
	prototypes # And finally, the prototypes

prototypes.depends = acknext addons
tools.depends = acknext addons
