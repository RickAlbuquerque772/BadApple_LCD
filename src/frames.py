import cv2
import numpy as np
from pathlib import Path

diretorio = "C:/Users/iamth/Downloads/badapple-frames-main/frames/output_"
NumFrames = 6572
#meu_arquivo = 'saida2.txt'
meu_arquivo = 'frame.h'

with open(meu_arquivo,'w',encoding='utf-8') as f:
    f.write("#ifndef FRAMES_H\n")
    f.write("#define FRAMES_H\n\n")
    f.write("#include <stdint.h>\n\n")
    f.write("#define NUM_FRAMES " + str(NumFrames) + "\n")
    f.write("#define FRAME_ROWS 32\n")
    f.write("#define FRAME_COLS 40\n\n")
    f.write("const uint8_t frames[NUM_FRAMES][FRAME_ROWS][FRAME_COLS] = {\n")
    #f.write("const uint8_t frames[][][] = {\n")
    f.close()

def definir_caminho(index):
    index_f = f"{index:04d}"
    index_str = str(index_f)
    caminho = diretorio + index_str +".jpg"
    caminho = Path(caminho)
    return str(caminho)

def gravar_saida(imagem_binaria, frame, colunas, linhas): 
    with open(meu_arquivo,'a',encoding='utf-8') as f:
        f.write("\t" + "/*" + " Frame " + str(frame) + " */" + "\n")
        f.write("\t{\n")
        for i in range(linhas):
            f.write("\t\t{" + str(imagem_binaria[i][0]))
            for j in range(1, colunas):
                f.write( "," + str(imagem_binaria[i][j]) )
            if(i == (linhas - 1)): # O primeiro já foi escrito
                f.write("}\n\t},\n")
            else:
                f.write("},\n")

def encerrar_saida():
    with open(meu_arquivo,'a',encoding='utf-8') as f:
        f.write("};\n\n")
        f.write("#endif")

for i in range(NumFrames):
    index = i
    frame = definir_caminho(index + 1) #OBS começa em 0001
    imagem_cinza = cv2.imread(frame,cv2.IMREAD_GRAYSCALE)

    novas_dimensoes = (40, 32)
    imagem_cinza_redim = cv2.resize(imagem_cinza,novas_dimensoes, interpolation=cv2.INTER_AREA)

    #_, joga fora o primeiro retorno da função cv2.threshold
    _, imagem_255 = cv2.threshold(imagem_cinza_redim,127, 255, cv2.THRESH_BINARY) # Converte para 0 e 255
    imagem_binaria = (imagem_255 // 255).astype(np.uint8)

    gravar_saida(imagem_binaria, i, novas_dimensoes[0], novas_dimensoes[1])

encerrar_saida()