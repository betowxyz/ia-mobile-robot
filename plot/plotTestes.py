import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

from plotvalues import plotTons, subPlotFinal, visibilityPlot

def plotMap(data, save, plot, fileName):
    plt.figure(figsize=(27.77,27.74), dpi=300)
    im = plt.imshow(data , interpolation='none') #
    colors =[
        im.cmap(im.norm(400)),
        im.cmap(im.norm(-255)),
        im.cmap(im.norm(200)),
        im.cmap(im.norm(0))
            ]
    patches = [
        mpatches.Patch(color=colors[0], label="Pontos de Guarda"),
        mpatches.Patch(color=colors[1], label="Obstáculo"),
        mpatches.Patch(color=colors[2], label="Rota"),
        mpatches.Patch(color=colors[3], label="Visível")
            ]
    plt.axis('off')
    if(save):
        plt.legend(handles=patches, loc=4, prop={'size': 30}) #size 40 for saving
        plt.savefig('C:\\Users\\Beto\\Desktop\\Projetos\\MobileRobot\\Img\\'+ fileName +'.png', bbox_inches="tight")
    if(plot):
        plt.legend(handles=patches, loc=4, prop={'size': 4}) #size 40 for saving
        plt.title("Occupancy Grid")
        plt.show()



# import numpy as np 

# plotPivit = np.array([
#     [-255, -255,-255, -255, -255, -255, -255, -255, -255, -255],
#     [-255, 0, 0, 0, 0, -255, -255, 0, 0, -255],
#     [-255, 0, 0, 0, 0, -255, -255, 0, 0, -255],
#     [-255, 0, 0, 0, 0, 0, 0, 0, 0, -255],
#     [-255, 0, 0, -255, -255, 0, 0, 0, 0, -255],
#     [-255, 0, 0, 0, -255, -255, 0, 0, 0, -255],
#     [-255, 0, 0, 0, 0, 0, 0, 0, 0, -255],
#     [-255, 0, 0, 0, 0, 0, 0, -255, 0, -255],
#     [-255, 0, 0, 0, 0, 0, 0, 0, 0, -255],
#     [-255, -255,-255, -255, -255, -255, -255, -255, -255, -255]
# ])

plotMap(data=subPlotFinal, save=False, plot=True, fileName ='aStarPadronizadoTonsNew2')