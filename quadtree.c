//Nikolas Lacerda

#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>     /* OpenGL functions */
#endif

unsigned int first = 1;
char desenhaBorda = 1;

QuadNode* newNode(int x, int y, int width, int height)
{
    QuadNode* n = malloc(sizeof(QuadNode));
    n->x = x;
    n->y = y;
    n->width = width;
    n->height = height;
    n->NW = n->NE = n->SW = n->SE = NULL;
    n->color[0] = n->color[1] = n->color[2] = 0;
    n->id = first++;
    return n;
}

QuadNode* geraNodes(Img* pic, float minDetail, int x, int y, int width, int height)
{

    QuadNode* node = newNode(x, y, width, height);
    node->status = PARCIAL;
    node->color[0] = 0;
    node->color[1] = 0;
    node->color[2] = 255;

    if(width == 1 && height == 1)
    {
           return node;
    }
    else
    {
        int size = width * height;
        int R = 0;
        int G = 0;
        int B = 0;

        for(int i=x; i< x + height; i++)
        {
            for(int j=y; j < y + width; j++)
            {
                int linha = (j * (pic->width * 3));
                int coluna = (i * 3);
                int linha_coluna = linha + coluna;

                R += pic->img[linha_coluna];
                G += pic->img[linha_coluna+1];
                B += pic->img[linha_coluna+2];
            }
        }

        int Rm = R / size;
        int Gm = G / size;
        int Bm = B / size;

        int dif = 0;

        for(int i=x; i < x + height; i++)
        {
            for(int j=y; j < y + width; j++)
            {
                int linha = (j * (pic->width * 3));
                int coluna = (i * 3);
                int linha_coluna = linha + coluna;

                R = pic->img[linha_coluna];
                G = pic->img[linha_coluna+1];
                B = pic->img[linha_coluna+2];

                dif += sqrt(pow((R - Rm), 2) + pow((G - Gm), 2) + pow((B - Bm), 2));
            }

        }

        int nivelDeDetalhe = dif / size;

        if(nivelDeDetalhe <= minDetail)
        {
            //node->status = CHEIO;
            //node->color[0] = Rm;
            //node->color[1] = Gm;
            //node->color[2] = Bm;
           return node;
        }
        else
        {
            int new_width = width / 2;
            int new_height = height / 2;

			node->NW = geraNodes(pic, minDetail, x, y, new_width, new_height);
			node->SW = geraNodes(pic, minDetail, x, y + new_width, new_width, new_height);
			node->NE = geraNodes(pic, minDetail, x + new_width, y, new_width, new_height);
			node->SE = geraNodes(pic, minDetail, x + new_width, y + new_width, new_width, new_height);
       }

    }

    return node;
}

QuadNode* geraQuadtree(Img* pic, float minDetail)
{

    int width = pic->width;
    int height = pic->height;

    QuadNode* raiz = geraNodes(pic, minDetail, 0, 0, width, height);

    return raiz;
}

// Limpa a memória ocupada pela árvore
void clearTree(QuadNode* n)
{
    if(n == NULL) return;
    if(n->status == PARCIAL)
    {
        clearTree(n->NE);
        clearTree(n->NW);
        clearTree(n->SE);
        clearTree(n->SW);
    }
    //printf("Liberando... %d - %.2f %.2f %.2f %.2f\n", n->status, n->x, n->y, n->width, n->height);
    free(n);
}

// Ativa/desativa o desenho das bordas de cada região
void toggleBorder() {
    desenhaBorda = !desenhaBorda;
    printf("Desenhando borda: %s\n", desenhaBorda ? "SIM" : "NÃO");
}

// Desenha toda a quadtree
void drawTree(QuadNode* raiz) {
    if(raiz != NULL)
        drawNode(raiz);
}

// Grava a árvore no formato do Graphviz
void writeTree(QuadNode* raiz) {
    FILE* fp = fopen("quad.dot", "w");
    fprintf(fp, "digraph quadtree {\n");
    if (raiz != NULL)
        writeNode(fp, raiz);
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nFim!\n");
}

void writeNode(FILE* fp, QuadNode* n)
{
    if(n == NULL) return;

    if(n->NE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NE->id);
    if(n->NW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NW->id);
    if(n->SE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SE->id);
    if(n->SW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SW->id);
    writeNode(fp, n->NE);
    writeNode(fp, n->NW);
    writeNode(fp, n->SE);
    writeNode(fp, n->SW);
}

// Desenha todos os nodos da quadtree, recursivamente
void drawNode(QuadNode* n)
{
    if(n == NULL) return;

    glLineWidth(0.1);

    if(n->status == CHEIO) {
        glBegin(GL_QUADS);
        glColor3ubv(n->color);
        glVertex2f(n->x, n->y);
        glVertex2f(n->x+n->width-1, n->y);
        glVertex2f(n->x+n->width-1, n->y+n->height-1);
        glVertex2f(n->x, n->y+n->height-1);
        glEnd();
    }

    else if(n->status == PARCIAL)
    {
        if(desenhaBorda) {
            glBegin(GL_LINE_LOOP);
            glColor3ubv(n->color);
            glVertex2f(n->x, n->y);
            glVertex2f(n->x+n->width-1, n->y);
            glVertex2f(n->x+n->width-1, n->y+n->height-1);
            glVertex2f(n->x, n->y+n->height-1);
            glEnd();
        }
        drawNode(n->NE);
        drawNode(n->NW);
        drawNode(n->SE);
        drawNode(n->SW);
    }
    // Nodos vazios não precisam ser desenhados... nem armazenados!
}

