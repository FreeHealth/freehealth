#!/usr/bin/python

# SETTINGS variables, please adjust to your needs

# path where the plugins are found. Only change if you move this script to another place.
pluginPath = "../plugins"

# this is a dictionary that contains a directory name (that contains the wizard.xml file) as key,
# e.g. "my_template_class"
# and the id of the xml field name as value, e.g. if the field xml tag is
#    <field name="PluginNamespace">
# then the dictionary value would be "PluginNamespace".
# If this xml field has a subtag "fieldcontrol" with an attribute "class" that has the value "QComboBox" - in short, if this field is a combo box, then this script updates the list values of that field with the new new list of plugin namespaces found in the FMF source code.
filesToUpdate = {
    #"formitem": ???,
    "pimpl_class": "PluginNamespace",
    "preferencespage": "PluginName",
    }

#---------------------- DONT CHANGE ANYTHING BELOW THIS LINE! -----------------------


import os
from os.path import join
import xml.etree.ElementTree as etree


def getNamespaces(pluginDirectory):
    """
    Searches in the *plugin.h files for the namespaces
    @param pluginDirectory string directory to start searching (usually "plugins")
    @returns a list of namespaces
    """
    namespacePool = []
    for name in os.listdir(pluginDirectory):
        if not os.path.isdir(os.path.join(pluginDirectory,name)):
            continue
        for filename in os.listdir(os.path.join(pluginDirectory,name)):
            # check if filename ends with
            if filename.count("plugin.h") == 1:
                #print(filename)
                try:
                    fp = open(os.path.join(pluginDirectory, name, filename))
                except IOError as e:
                    print("Error accessing %s." % filename)

                else:
                    lines = fp.readlines()
                    for line in lines:

                        line = line.strip()

                        # ignore "Internal" namespace
                        if line.find("Internal") > -1:
                            continue

                        words = line.split()
                        if words:
                            if words[0] == "namespace":

                                namespace = words[1]
                                # workaround for not having a whitespace between the namespace
                                # and the "{"
                                if namespace.find("{"):
                                    namespace = namespace.replace("{", "")

                                if namespace == "Internal":
                                    continue

                                # now we have our namespace!
                                print("found plugin file '%s': namespace '%s'" % (filename, namespace))
                                # add it to the pool
                                namespacePool.append(namespace)

    return namespacePool

def replaceXmlFileValues(filename):
    pass


if __name__ == "__main__":
    namespaces = getNamespaces(pluginPath)
    namespaces.sort()
    for directory in filesToUpdate.keys():
        if not os.path.isdir(directory):
            print("%s is no directory." % directory)
            print os.getcwd()
            continue

        filename = os.path.join(directory, "wizard.xml")
        try:
            tree = etree.parse(filename)
        except IOError as e:
            print("Error accessing %s." % filename)
        else:
            root = tree.getroot()
            #find all elements that have a name attribute that is listed in the settings dict
            for field in root.findall("fields/field[@name='%s']" % filesToUpdate[directory]):
                #print field.attrib
                targetElement = field.find("fieldcontrol")
                if targetElement is None:
                    continue
                targetElement.set("combochoices", ",".join(namespaces))
                tree.write(filename)


