#!/usr/bin/env python

# python ./CsvToXml.py "Helio Localization - v1.0.csv" output.xml en ru de it es fr br

import sys
import csv
import xml.etree.cElementTree as ET

from xml.dom import minidom
from xml.dom.minidom import parse, parseString

def prettify(elem):
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

#reload(sys)  
#sys.setdefaultencoding('utf8')

if len(sys.argv) < 4:
    print("Usage: python ./csvToXml.py input.csv output.xml en ru de it etc")
    quit()

languages = sys.argv[3:]
print(languages)

root = ET.Element("Translations")

for language in languages:
    locale = ET.SubElement(root, "Locale")
    locale.set("Id", language)
    with open(sys.argv[1], "r") as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            key = row["ID"]
            value = row[language]
            if key == "::locale":
                locale.set("Name", value)
            elif key == "::fallback":
                locale.set("Fallback", value)
            elif key == "::plural":
                ET.SubElement(locale, "PluralForms", Equation = value)
            else:
                if value != "" and key != "":
                    if "{x}" in value.lower() and len(value.splitlines()) > 1:
                        # Plural
                        pluralLiteral = ET.SubElement(locale, "PluralLiteral", Name = key)
                        for index, literal in enumerate(value.splitlines()):
                            fixedLiteral = literal.replace("{X}", "{x}")
                            ET.SubElement(pluralLiteral, "Translation", Name = fixedLiteral, PluralForm = str(index + 1))
                    else:
                        # Singular
                        fixedValue = value.replace("{X}", "{x}")
                        ET.SubElement(locale, "Literal", Name = key, Translation = fixedValue)

tree = ET.ElementTree(root)

fileHandle = open(sys.argv[2], "wb")
fileHandle.write(prettify(tree.getroot()).encode())
fileHandle.close()
