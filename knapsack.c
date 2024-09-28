#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h> //ajout de cette bibliothéque pour avoir la constante INT_MAX (l'infini)

// Fonction utilitaire pour obtenir le maximum et le minimum de deux nombres.
int max(int a, int b) { return (a > b) ? a : b; }
int min(int a, int b) { return (a < b) ? a : b; }


// Définition de la structure pour les objets.
typedef struct {
    unsigned int weight;
    unsigned int value;
} Item;


// Variables globales pour l'algorithme de backtracking
// (ATTENTION: nous utilisons les variables globales dans cette version pour suivre le schéma vu en cours et en td)
Item* g_itmes;             // tableau d'objets
int g_n;                   // nombre d'objets
int g_W;            // capacité du sac
int g_bestValue = 0;       // meilleure valeur trouvée jusqu'à présent
int g_currentValue = 0;    // valeur courante du sac
int g_currentWeight = 0;   // poids courant du sac


//fonction pour calculer la somme de valeur restante a partir de l'index
int borne_sup(int current_value,int index){
    int i, somme = current_value;
    for(i = index; i < g_n; i++)
        somme += g_itmes[i].value;
    return somme;
}

// implémentation récursive de l'algorithme de backtracking
void knapsackBT1(int idx) {
    // Si l'indice actuel est hors le tableau ou si le sac à dos est plein, mettre à jour bestValue si nécessaire
    if (idx == g_n || g_currentWeight == g_W) {
        g_bestValue = max(g_bestValue, g_currentValue);
        return;
    }
    
    if(borne_sup(g_currentValue, idx) < g_bestValue){
        return;
    }
    // sauter l'objet courant
    knapsackBT1(idx + 1);

    // Inclure l'objet actuel s'il ne dépasse pas la capacité du sac à dos , et si ca satisfait la condition d'élagage
    if (g_currentWeight + g_itmes[idx].weight <= g_W){ //&& borne_sup(g_currentValue , idx) >= g_bestValue) {
        // Mettre à jour currentWeight et currentValue avant l'appel récursif
        g_currentWeight += g_itmes[idx].weight;
        g_currentValue += g_itmes[idx].value;

        knapsackBT1(idx + 1);

        // Annuler la décision d'inclure l'objet (backtracking)
        g_currentWeight -= g_itmes[idx].weight;
        g_currentValue -= g_itmes[idx].value;
    }
}


// Algorithme de backtracking sans variables globales
int knapsackBTUtil(Item items[], int n, int W, int idx, int currentValue, int *bestValue) {
    if (idx == n || W == 0) {
        if (currentValue > *bestValue)
            *bestValue = currentValue;
        return *bestValue;
    }
    //calcul de la borne sup: somme des valeurs de tout les elements restants et la currentValue
    int borneSup = currentValue;
    for(int i = idx; i < n; i++){
        borneSup += items[i].value;
    }
    //elagage: si borne sup et inferieures ou égale a la bestValue ca sert a rien d'explorer la branche
    if(borneSup <= *bestValue){
          return *bestValue;
    }

    // Ne pas inclure l'objet idx
    knapsackBTUtil(items, n, W, idx + 1, currentValue, bestValue);

    // Inclure l'objet idx si le poids le permet
    if (items[idx].weight <= W ) {
        knapsackBTUtil(items, n, W - items[idx].weight, idx + 1, currentValue + items[idx].value, bestValue);
    }

    return *bestValue;
}

int knapsackBT2(Item items[], int n, int W) {
    int bestValue = 0;
    return knapsackBTUtil(items, n, W, 0, 0, &bestValue);
}





// Fonction de programmation dynamique pour le problème du sac à dos 0/1.
int knapsackDP(int W, Item items[], int n) {
    int i, w;
    int **K = (int**)malloc((n+1) * sizeof(int*));
    for(i = 0; i <= n; i++)
        K[i] = (int*)malloc((W+1) * sizeof(int));

    for(i = 0; i <= n; i++) {
        for (w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                K[i][w] = 0;
            else if (items[i-1].weight <= w)
                K[i][w] = max(items[i-1].value + K[i-1][w-items[i-1].weight], K[i-1][w]);
            else
                K[i][w] = K[i-1][w];
        }
    }

    int result = K[n][W];
    for (i = 0; i <= n; i++)
        free(K[i]);
    free(K);
    return result;
}

//focntion de programmation dynamique pour le problème du sac à dos avec les valeurs
int knapsackDP_Value(int W, Item items[], int n){
    //initialisation
    int V = 0, i,v;
    int *dp;
    for(i = 0; i < n; i++)
        V += items[i].value;
    dp = malloc((V+1) * sizeof(int));

    if(dp == NULL){
        perror("erreur d'allocation pour dp");
        exit(2);
    }
    dp[0] = 0;
    for(i = 1; i <= V; i++){
        dp[i] = INT_MAX;
    }

    //construction de la solution
    for(i = 0; i < n; i++){
        for(v = V; v > items[i].value; v--){
            dp[v] = min(dp[v], dp[v - items[i].value] + items[i].weight);
        }
    }
    //extraction de la solution
    int max_value = 0;
    for(i = V; i > 0; i--){
        if(dp[i] <= W ){
            max_value = i;
            break;
        }
    }
    free(dp);
    return max_value;

}

//Fonction de programmation dynamique pour le problème du sac à dos 0/1  avec poids minimum
int knapsackDP_min(int W, int M, Item items[], int n) {
    int i, w;
    int **dp = (int**)malloc((n + 1) * sizeof(int*));
    for(i = 0; i <= n; i++)
        dp[i] = (int*)malloc((W + 1) * sizeof(int));

    // Initialisation de la table de programmation dynamique
    for(i = 0; i <= n; i++) {
        for(w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                dp[i][w] = 0;
            else if (items[i-1].weight <= w)
                dp[i][w] = max(items[i-1].value + dp[i-1][w - items[i-1].weight], dp[i-1][w]);
            else
                dp[i][w] = dp[i-1][w];
        }
    }

    // Recherche de la valeur maximale qui satisfait la contrainte de poids minimum
    int result = 0;
    for (w = M; w <= W; w++) {
        if (dp[n][w] > result) {
            result = dp[n][w];
        }
    }

    // Libération de la mémoire allouée
    for(i = 0; i <= n; i++)
        free(dp[i]);
    free(dp);

    return result;
}

//fonctions auxiliaire pour trier le tableau pour l'algorithmes gloutons
void fusion(Item tableau[], int debut, int milieu, int fin) {
    int taille1 = milieu - debut + 1;
    int taille2 = fin - milieu;

    // Création des tableaux temporaires pour stocker les sous-tableaux
    Item *gauche = malloc(taille1 * sizeof(Item));
    Item *droite = malloc(taille2 * sizeof(Item));

    // Copie des données dans les tableaux temporaires
    for (int i = 0; i < taille1; i++)
        gauche[i] = tableau[debut + i];
    for (int j = 0; j < taille2; j++)
        droite[j] = tableau[milieu + 1 + j];

    // Fusion des tableaux temporaires dans le tableau final
    int i = 0, j = 0, k = debut;
    while (i < taille1 && j < taille2) {
        if ((float)gauche[i].value / gauche[i].weight <= (float)droite[j].value / droite[j].weight) {
            tableau[k] = gauche[i];
            i++;
        } else {
            tableau[k] = droite[j];
            j++;
        }
        k++;
    }

    // Copie des éléments restants de gauche et de droite
    while (i < taille1) {
        tableau[k] = gauche[i];
        i++;
        k++;
    }
    while (j < taille2) {
        tableau[k] = droite[j];
        j++;
        k++;
    }

    // Libération de la mémoire des tableaux temporaires
    free(gauche);
    free(droite);
}

// Fonction récursive pour trier un tableau avec l'algorithme de tri fusion
void tri_fusion(Item tableau[], int debut, int fin) {
    if (debut < fin) {
        int milieu = debut + (fin - debut) / 2;
        tri_fusion(tableau, debut, milieu);
        tri_fusion(tableau, milieu + 1, fin);
        fusion(tableau, debut, milieu, fin);
    }
}


//algorithmes gloutons
int knapsack_greedy(int W, Item items[], int n){
    //on trie le tableau
    tri_fusion(items, 0, n-1);

    //on ajoute les elements
    int i=0, total_weight = 0, best_value = 0;
    for(i = 0; i < n; i++){
        if((total_weight + items[i].weight) <= W)
            total_weight += items[i].weight;
            best_value += items[i].value;
    }
    return best_value;
}




int main() {
    FILE *fp = fopen("instances.csv", "r");
    FILE *out = fopen("average_times.csv", "w");
    FILE *sol = fopen("solutions.csv", "w");
    if (!fp || !out || !sol) {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return 1;
    }

    // Variables pour stocker les somme des temps et compter les instances par taille de problème
    double sumTimeBT1 = 0, sumTimeBT2 = 0, sumTimeDP = 0, sumTimeDP_v = 0, sumTimeDP_min = 0, sumTime_grd = 0;//ajout pour DP_Value et DP_min et Greedy
    int count = 0, currentN = -1;

    char line[4096];
    while (fgets(line, 4096, fp)) {
        int W, n;
        sscanf(line, "%d,%d", &W, &n);

        // Vérifiez si nous passons à une nouvelle taille de problème
        if (n != currentN) {
            if (count > 0) {
                // Écrivez les moyennes pour la taille de problème précédente
                fprintf(out, "%d, %f, %f, %f, %f, %f, %f\n",
                        currentN, sumTimeBT1 / count, sumTimeBT2 / count, sumTimeDP / count, sumTimeDP_v / count, sumTimeDP_min / count, sumTime_grd / count);
            }
            // Réinitialisez les compteurs pour la nouvelle taille
            currentN = n;
            sumTimeBT1 = sumTimeBT2 = sumTimeDP = sumTimeDP_v = sumTimeDP_min = sumTime_grd = 0;
            count = 0;
        }

        Item items[n];
        char *token = strtok(line, ",");
        token = strtok(NULL, ","); // Passer les deux premiers tokens (W et n)
        token = strtok(NULL, ","); //ligne ajoutée par moi 
        for (int i = 0; i < n && token != NULL; i++) {
            sscanf(token, "%d", &items[i].weight);
            token = strtok(NULL, ",");
            sscanf(token, "%d", &items[i].value);
            token = strtok(NULL, ",");
        }
        // Ici, insérez la logique pour configurer et appeler vos fonctions de sac à dos.
        printf("n=%d, W=%d\n", n, W);

        g_itmes = items;
        g_n = n;
        g_W = W;
        g_bestValue = 0;       // meilleure valeur trouvée jusqu'à présent
        g_currentValue = 0;    // valeur courante du sac
        g_currentWeight = 0;

        clock_t start = clock();
        knapsackBT1(0);
        int maxValBT1 = g_bestValue;
        clock_t end = clock();
        double timeBT1 = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeBT1 += timeBT1;

        start = clock();
        int maxValBT2 = knapsackBT2(items, n, W);
        end = clock();
        double timeBT2 = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeBT2 += timeBT2;

        start = clock();
        int maxValDP = knapsackDP(W, items, n);
        end = clock();
        double timeDP = (double)(end - start) / CLOCKS_PER_SEC;
        sumTimeDP += timeDP;

        start = clock();
        int maxValDP_v = knapsackDP_Value(W, items, n);
        end = clock();
        double timeDP_v = (double) (end - start) / CLOCKS_PER_SEC;
        sumTimeDP_v += timeDP_v;

        start = clock();
        int maxValDP_min = knapsackDP_min(W, W/2 - ((n*2)%25) , items, n);
        end = clock();
        double timeDP_min = (double) (end - start) / CLOCKS_PER_SEC;
        sumTimeDP_min += timeDP_min;

        start = clock();
        int maxVal_greedy = knapsack_greedy(W, items, n);
        end = clock();
        double time_greedy = (double) (end - start) / CLOCKS_PER_SEC;
        sumTime_grd += time_greedy;

        count++;

        fprintf(sol, "%d, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d\n",
            n, W, timeBT1, maxValBT1, timeBT2, maxValBT2, timeDP, maxValDP, timeDP_v, maxValDP_v, timeDP_min, maxValDP_min, time_greedy, maxVal_greedy);
    }

    // N'oubliez pas d'écrire les moyennes pour la dernière taille de problème
    if (count > 0) {
        fprintf(out, "%d, %f, %f, %f, %f, %f, %f\n",
                currentN, sumTimeBT1 / count, sumTimeBT2 / count, sumTimeDP / count, sumTimeDP_v / count, sumTimeDP_min / count, sumTime_grd / count);
    }

    fclose(fp);
    fclose(out);
    return 0;
}