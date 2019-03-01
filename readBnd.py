
import sys
import json
import struct
import glob

from collections import namedtuple

importFilenames = sys.argv[1:]

for importFilename in importFilenames:
    exportFilename = importFilename.replace(".bnd", ".obj").replace(".BND", ".obj")
    print "reading " + importFilename
    try:
        with open(importFilename, "rb") as input_fd:
            rawData = input_fd.read()
        bndHeaderFmt = "<4sL4s2L4sL"
        offset = 0
        objectFmt = "<4L3L2L"
        primitiveFmt = "<6H12B"
        vertexFmt = "<4h"
        normalFmt = "<4h"
        (fileHeader, fileLength, dataHeader, num1, num2, tmdHeader, tmdLength) = struct.unpack_from(bndHeaderFmt, rawData, offset)
        print (tmdHeader, tmdLength + struct.calcsize(bndHeaderFmt))
        offset += struct.calcsize(bndHeaderFmt)
        objects = []
        for i in range(1):
            verts = []
            normals = []
            primitives = []

            (topVert, numVerts, topNormal, numNormals, unk1, unk2, unk3, topPrimitive, numPrimitives) = struct.unpack_from(objectFmt, rawData, offset)
            originalOffset = offset
            offset += struct.calcsize(objectFmt)
            offset = originalOffset + topVert
            for x in range(numVerts):
                vert = struct.unpack_from(vertexFmt, rawData, offset)
                offset += struct.calcsize(vertexFmt)
                verts.append(vert)

            offset = originalOffset + topNormal
            print numNormals
            for x in range(numNormals):
                normal = struct.unpack_from(normalFmt, rawData, offset)
                offset += struct.calcsize(normalFmt)
                normals.append(normal)
            print numPrimitives
            for x in range(numPrimitives):
                prim = struct.unpack_from(primitiveFmt, rawData, offset)
                offset += struct.calcsize(primitiveFmt)
                primitives.append(prim)
                print prim
            objects.append((verts, normals, primitives))
        print offset

        with open(exportFilename,"w") as shapeFile:
            faceIndex = 0
            for index, object in enumerate(objects):
                shapeFile.write("o " + str(index))
                shapeFile.write("\n")
                verts, normals, faces = object
                for vertex in verts:
                    shapeFile.write("\tv " + str(float(vertex[0])) + " " + str(float(vertex[1])) + " " + str(float(vertex[2])) + "\n")

                for polygon in faces:
                    shapeFile.write("\tf ")
                    shapeFile.write(str(polygon[0] + 1 + faceIndex) + " ")
                    shapeFile.write(str(polygon[1] + 1 + faceIndex) + " ")
                    shapeFile.write(str(polygon[2] + 1 + faceIndex) + " ")
                    shapeFile.write("\n")
                    shapeFile.write("\tf ")
                    shapeFile.write(str(polygon[1] + 1 + faceIndex) + " ")
                    shapeFile.write(str(polygon[2] + 1 + faceIndex) + " ")
                    shapeFile.write(str(polygon[3] + 1 + faceIndex) + " ")
                    shapeFile.write("\n")



                faceIndex += len(object[0])

    except Exception as e:
        print e
