#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LIBROS 10 // Define el límite máximo de libros/nodos del catálogo

// Estructura que representa un libro (nodo del grafo)
typedef struct {
    int id;           
    char titulo[100]; 
    char autor[100];  
    char genero[50];  
} tlibro;

// Estructura que representa el Grafo de Adyacencia para las recomendaciones
typedef struct {
    int matriz_arco[MAX_LIBROS][MAX_LIBROS]; 
    tlibro * libros[MAX_LIBROS];             
} tGrafoAdyacencia;

// Declaración global del grafo de recomendaciones
tGrafoAdyacencia * grafo_recomendaciones;

/* ======================= PROTOTIPOS DE FUNCIONES ======================= */

void IniciarGrafo(tGrafoAdyacencia **);
void MostrarGrafo(tGrafoAdyacencia **);
void CrearLibro(tGrafoAdyacencia **, int, int, const char *, const char *, const char *);
void recorrerRecomendacionesDesde(tGrafoAdyacencia * grafo, int idInicio);
void DFSRecomendaciones(tGrafoAdyacencia * grafo, int idActual, int visitados[]);
int cargarVectorLibros(tGrafoAdyacencia * grafo, tlibro * v[]);
void ordenarLibrosPorTitulo(tlibro * v[], int n);
void mostrarLibrosOrdenados(tlibro * v[], int n);
int buscarLibroPorTituloSecuencial(tlibro * v[], int n, const char * titulo);
void MostrarLibroCargado(tlibro * lib);

/* =========================== FUNCION PRINCIPAL =========================== */

int main()
{
    int opcion;
    int idInicio;
    tlibro * vectorLibros[MAX_LIBROS];
    int cantidad;
    char tituloBuscado[100];
    int pos;

    // Inicializar grafo y cargar datos fijos de ejemplo
    IniciarGrafo(&grafo_recomendaciones);

    // Carga de libros y relaciones iniciales
    CrearLibro(&grafo_recomendaciones, 0, -1, "La Furia y el Amanecer", "Renee Ahdieh", "Fantasia Juvenil");
    CrearLibro(&grafo_recomendaciones, 1, 0, "Una Corte de Rosas y Espinas", "Sarah J. Maas", "Fantasia Juvenil");
    CrearLibro(&grafo_recomendaciones, 2, 1, "Trono de Cristal", "Sarah J. Maas", "Fantasia Epica");
    CrearLibro(&grafo_recomendaciones, 3, 4, "Dune", "Frank Herbert", "Ciencia Ficcion");
    CrearLibro(&grafo_recomendaciones, 4, 3, "Fundacion", "Isaac Asimov", "Ciencia Ficcion");


    /* MENU PRINCIPAL */
    do
    {
        printf("\n================ MENU CATALOGO DE LIBROS ================\n");
        printf("1) Mostrar catalogo y recomendaciones (Grafo)\n");
        printf("2) Recorrer recomendaciones (DFS recursivo) desde un libro\n");
        printf("3) Mostrar libros ordenados por titulo\n");
        printf("4) Buscar libro por titulo (Busqueda Secuencial)\n");
        printf("0) Salir\n");
        printf("Opcion: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
            case 1:
                MostrarGrafo(&grafo_recomendaciones);
                break;

            case 2:
                printf("\nIngrese el ID de inicio para el recorrido DFS: ");
                scanf("%d", &idInicio);
                recorrerRecomendacionesDesde(grafo_recomendaciones, idInicio);
                break;

            case 3:
                cantidad = cargarVectorLibros(grafo_recomendaciones, vectorLibros);
                if (cantidad == 0)
                {
                    printf("\nNo hay libros cargados.\n");
                }
                else
                {
                    ordenarLibrosPorTitulo(vectorLibros, cantidad);
                    mostrarLibrosOrdenados(vectorLibros, cantidad);
                }
                break;

            case 4:
                cantidad = cargarVectorLibros(grafo_recomendaciones, vectorLibros);
                if (cantidad == 0)
                {
                    printf("\nNo hay libros cargados.\n");
                }
                else
                {
                    printf("\nIngrese el titulo a buscar (Secuencial): ");
                    fflush(stdin); 
                    scanf(" %99[^\n]", tituloBuscado);

                    pos = buscarLibroPorTituloSecuencial(vectorLibros, cantidad, tituloBuscado);
                    if (pos != -1)
                    {
                        printf("\n[Secuencial] Encontrado '%s' en posicion %d (ID %d)\n",
                               vectorLibros[pos]->titulo, pos, vectorLibros[pos]->id);
                    }
                    else
                    {
                        printf("\n[Secuencial] Libro no encontrado.\n");
                    }
                }
                break;

            case 0:
                printf("\nSaliendo del programa...\n");
                break;

            default:
                printf("\nOpcion invalida. Intente nuevamente.\n");
                break;
        }

    } while(opcion != 0);

    return 0;
}

void IniciarGrafo(tGrafoAdyacencia ** ppgrafo)
{
    int i, j;

    *ppgrafo = (tGrafoAdyacencia *)malloc(sizeof(tGrafoAdyacencia));
    if (*ppgrafo == NULL) {
        printf("Error al asignar memoria para la raiz del catalogo\n");
        exit(EXIT_FAILURE);
    }


    for (i = 0; i < MAX_LIBROS; i++)
    {
        for (j = 0; j < MAX_LIBROS; j++)
        {
            (*ppgrafo)->matriz_arco[i][j] = 0;
        }
    }


    for (i = 0; i < MAX_LIBROS; i++)
    {
        (*ppgrafo)->libros[i] = NULL;
    }
}


void MostrarGrafo(tGrafoAdyacencia ** ppgrafo)
{
    int i, j;

    if (*ppgrafo == NULL) {
        printf("Error: Grafo no inicializado.\n");
        return;
    }

    printf("\n========= CATALOGO Y RECOMENDACIONES =========\n");

    for(i = 0; i < MAX_LIBROS; i++)
    {
        tlibro *libro_origen = (*ppgrafo)->libros[i];

        if(libro_origen != NULL)
        {
            printf("\n--- LIBRO ID %d: %s ---\n", libro_origen->id, libro_origen->titulo);
            printf("Autor: %s, Genero: %s\n", libro_origen->autor, libro_origen->genero);

            
            if (libro_origen->pariente != -1 &&
                libro_origen->pariente >= 0 &&
                libro_origen->pariente < MAX_LIBROS)
            {
                tlibro *par = (*ppgrafo)->libros[libro_origen->pariente];
                if (par != NULL)
                    printf("RELACION DIRECTA (Pariente): %s\n", par->titulo);
            }

            
            printf("RECOMENDACIONES (Arcos):\n");
            for(j = 0; j < MAX_LIBROS; j++)
            {
                if ((*ppgrafo)->matriz_arco[i][j] == 1 && i != j)
                {
                    tlibro *dest = (*ppgrafo)->libros[j];
                    if (dest != NULL)
                        printf("  -> Recomienda: %s (ID %d)\n", dest->titulo, dest->id);
                }
            }
        }
    }
    printf("\n============================================\n");
}

void CrearLibro(tGrafoAdyacencia ** ppgrafo, int nuevo_id, int pariente,
                const char *nuevo_titulo, const char *nuevo_autor, const char *nuevo_genero)
{
    tlibro * aux;

    if (*ppgrafo == NULL || nuevo_id < 0 || nuevo_id >= MAX_LIBROS) return;

    aux = (tlibro *)malloc(sizeof(tlibro));
    if (aux == NULL)
    {
        printf("Error al asignar memoria para el libro.\n");
        return;
    }

    aux->id = nuevo_id;
    aux->pariente = pariente;
    strcpy(aux->titulo, nuevo_titulo);
    strcpy(aux->autor, nuevo_autor);
    strcpy(aux->genero, nuevo_genero);

    (*ppgrafo)->libros[nuevo_id] = aux;

    
    if (pariente >= 0 && pariente < MAX_LIBROS)
    {
        (*ppgrafo)->matriz_arco[nuevo_id][pariente] = 1;
        (*ppgrafo)->matriz_arco[pariente][nuevo_id] = 1;
    }
}

void recorrerRecomendacionesDesde(tGrafoAdyacencia * grafo, int idInicio)
{
    int visitados[MAX_LIBROS];
    int i;

    if (grafo == NULL || idInicio < 0 || idInicio >= MAX_LIBROS || grafo->libros[idInicio] == NULL)
    {
        printf("Error: id de inicio invalido o grafo no inicializado.\n");
        return;
    }

    for (i = 0; i < MAX_LIBROS; i++)
        visitados[i] = 0;

    printf("Recorriendo recomendaciones (DFS) desde: %s (ID %d)\n",
           grafo->libros[idInicio]->titulo, idInicio);

    DFSRecomendaciones(grafo, idInicio, visitados);
}


void DFSRecomendaciones(tGrafoAdyacencia * grafo, int idActual, int visitados[])
{
    int j;
    tlibro * actual;


    if (visitados[idActual])
        return;

    visitados[idActual] = 1;  

    actual = grafo->libros[idActual];
    if (actual == NULL)
        return;

    printf("  Visitando: %s (ID %d)\n", actual->titulo, actual->id);

    for (j = 0; j < MAX_LIBROS; j++)
    {
        if (grafo->matriz_arco[idActual][j] == 1 && o
            !visitados[j] &&                       
            grafo->libros[j] != NULL)              
        {
            DFSRecomendaciones(grafo, j, visitados);
        }
    }
}

int cargarVectorLibros(tGrafoAdyacencia * grafo, tlibro * v[])
{
    int i, k = 0;

    for (i = 0; i < MAX_LIBROS; i++)
    {
        if (grafo->libros[i] != NULL)
        {
            v[k] = grafo->libros[i];
            k++;
        }
    }
    return k; 
}

void ordenarLibrosPorTitulo(tlibro * v[], int n)
{
    int i, j, posMin;
    tlibro * aux;

    for (i = 0; i < n - 1; i++)
    {
        posMin = i;

        for (j = i + 1; j < n; j++)
        {
            // Compara títulos. Si v[j] es alfabéticamente menor, actualiza posMin.
            if (strcmp(v[j]->titulo, v[posMin]->titulo) < 0)
            {
                posMin = j;
            }
        }

        // Intercambio si se encuentra un título alfabéticamente menor
        if (posMin != i)
        {
            aux = v[i];
            v[i] = v[posMin];
            v[posMin] = aux;
        }
    }
}


void mostrarLibrosOrdenados(tlibro * v[], int n)
{
    int i;
    printf("\n========== LIBROS ORDENADOS POR TITULO ==========\n");

    for (i = 0; i < n; i++)
    {
        printf("[%d] %s - %s (%s)\n",
               v[i]->id,
               v[i]->titulo,
               v[i]->autor,
               v[i]->genero);
    }
}

int buscarLibroPorTituloSecuencial(tlibro * v[], int n, const char * titulo)
{
    int i;

    for (i = 0; i < n; i++)
    {
        // Compara el título
        if (strcmp(v[i]->titulo, titulo) == 0)
        {
            return i; // Encontrado, devuelve la posición
        }
    }

    return -1; // No encontrado
}

void MostrarLibroCargado(tlibro * lib)
{
    if (lib == NULL)
        return;

    printf("\n[LIBRO CARGADO]\n");
    printf(" ID       : %d\n", lib->id);
    printf(" Pariente : %d\n", lib->pariente);
    printf(" Titulo   : %s\n", lib->titulo);
    printf(" Autor    : %s\n", lib->autor);
    printf(" Genero   : %s\n", lib->genero);

}

