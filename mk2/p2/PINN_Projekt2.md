Proszę na swojej lokalnej kopii (np. w Google Colab) zmodyfikować kod

https://github.com/pmaczuga/pinn-notebooks/blob/master/PINN_thermal_inversion.ipynb

w taki sposób żeby odpowiadał on symulacji działka atmosferycznego którą wykonywali Państwo w projekcie IGA-ADS.

Proszę jako rozwiązanie wsadzić historię uruchomienia np. z Google Colab

Proszę przygotować raport w pliku pdf z następującymi elementami

1. Fragmenty kodu pythona zmodyfikowane w kodzie inwersji termicznej żeby uruchomić Państwa symulator PINN

2. Rysunek stanu początkowego. Opis stanu początkowego. Kod który definiuje stan początkowy

3. Szczegółowy opis definicji działka atmosferycznego. . Kod który definiuje działko atmosferyczne

4. Szczegółowy opis zastosowanego pola adwekcji (wzór na ruch powietrza generowany przez działko atmosferyczne plus ewentualnie ruch powietrza spowodowany gradientem temperatory) [wydaje mi się że lepiej gradient temperatury wyzerować ponieważ strzelanie działkiem to inna skala czasowa]

5. Ewentualnie wariacje na temat liczby działek atmosferycznych oraz jej wpływ na wynik symulacji

6. Wykresy zbieżności procesu uczenia (loss residual, loss init, loss boundary)

7. Filmik z przebiegu całej symulacji

8. 10 klatek przebiegu symulacji

9. Jesli PINN nie chcę się nauczyć warto rozważyć podpięcie algorytmu SoftAdapt

https://github.com/dr-aheydari/SoftAdapt

https://arxiv.org/abs/1912.12355