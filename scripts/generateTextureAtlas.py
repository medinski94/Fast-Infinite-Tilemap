import os
import math
import glob
import argparse
import numpy as np
import cv2 as cv

def isDir(string):
    if os.path.isdir(string):
        return string
    else:
        raise NotADirectoryError(string)

def parseArguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('--in', dest='indir', type=isDir, help='path to input textures', required=True)
    parser.add_argument('--out', dest='outdir', type=isDir, help='path to output atlas', default='.')
    parser.add_argument('--size', dest='size', type=int, help='the width and height of the texture', required=True)
    parser.add_argument('--cols', dest='cols',type=int, help='Columns in atlas', required=True)
    parser.add_argument('--rows', dest='rows', type=int, help='Rows in atlas', required=True)
    parser.add_argument('--mipmaps', dest='mipmaps', type=bool, help='generate mipmaps', nargs='?', default=False, const=True)
    parser.add_argument('--padding', dest='padding', type=bool, help='apply padding to textures', nargs='?', default=False, const=True)
    parser.add_argument('--clamp', dest='clamp', type=bool, help='clamp textures in atlas', nargs='?', default=False, const=True)
    parser.add_argument('--smooth', dest='smooth', type=bool, help='use bilinear interpolation', nargs='?', default=False, const=True)
    args = parser.parse_args()
    return args

def loadTextures(sourceDir):
    textures = []
    for img in glob.glob(sourceDir + '/' + '*.jpg', recursive = False):
        textures.append(cv.imread(img))

    return textures;

def generateAtlas(textures, textureSize, atlasCols, atlasRows, mipmaps, padding, smooth, clamp):
    atlases = []
    numTextures = min(atlasCols * atlasRows, len(textures))
    margin = 0 if not padding else 1 if not mipmaps else textureSize
    levels = int(1 if not mipmaps else math.log2(textureSize) + 1)
    
    for i in range(levels):
        atlasWidth = (textureSize + margin * 2) * atlasCols
        atlasHeight = (textureSize + margin * 2) * atlasRows
        
        atlas = np.zeros((atlasHeight, atlasWidth, 3), np.uint8)
        for i in range(numTextures):
            x = (i % (atlasCols)) * (textureSize + margin * 2)
            y = (i // (atlasCols)) * (textureSize + margin * 2)
            
            textures[i] = cv.resize(textures[i], (textureSize, textureSize), interpolation = cv.INTER_AREA if smooth else cv.INTER_NEAREST)
            tile = cv.copyMakeBorder(textures[i], margin, margin, margin, margin, cv.BORDER_REPLICATE if clamp else cv.BORDER_CONSTANT, value = [255,255,255])

            atlas[y:y + tile.shape[0], x:x + tile.shape[1]] = tile

        atlases.append(atlas)
        
        textureSize //= 2
        margin //= 2
    
    return atlases

def writeAtlases(atlases, out):
    for i in range(len(atlases)):
        cv.imwrite(out + "/" + "%s-%d.png" % ("out", i), atlases[i])
    
args = parseArguments()

inDir = str(args.indir)
outDir = str(args.outdir)
size = int(args.size)
cols = int(args.cols)
rows = int(args.rows)
mipmaps = bool(args.mipmaps)
padding = bool(args.padding)
clamp = bool(args.clamp)
smooth = bool(args.smooth)

textures = loadTextures(inDir)
atlases = generateAtlas(textures, size, cols, rows, mipmaps, padding, smooth, clamp)
writeAtlases(atlases, outDir)
 