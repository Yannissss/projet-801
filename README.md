# projet-801
Lien Github : https://github.com/Yannissss/projet-801/
Implémentation de Gauss-Seildel avec OpenMP et Kokkos

Utilisation :

./gauss-seidel [image] 
               -v : mode affichage simple 
               -h : affiche l'aide 
               -p : active le mode parallèle
               N (un entier pour le nombre de taches)
               
Compilation :

La version Kokkos se trouve dans le répertoire kokkos/

Pour compiler chaque version il suffit de faire
source env.sh
make clean
make
