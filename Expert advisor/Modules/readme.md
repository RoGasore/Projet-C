# Description des Modules

    1. Filtre TK Boss.mqh
    Ce module est un filtre de tendance. Il utilise l'indicateur personnalisé "TK boss Supply & Demand" pour détecter le dernier signal (d'achat ou de vente) et envoie cet état (`BUY`, `SELL` ou `NEUTRE`) au module de décision. Il permet de s'assurer que le trading se fait dans la direction de la tendance principale identifiée par cet indicateur. Tu peux le remplacer par n'importe quel autre indicateur de tendance en configurant son nom et ses buffers dans la table de données accessible via `Ctrl+D` sur MetaTrader.

    2. ForexLife.mqh
    Ce module est un **confirmateur de tendance**. Il lit les données de l'indicateur "ForexLife Trend Direction" pour déterminer si la tendance est haussière ou baissière. Il agit comme un second avis, renforçant ou annulant le signal des autres modules. Il transmet son état (`BUY` ou `SELL`) au module de décision.

    2. Oscillator.mqh
    Ce module sert de détecteur de momentum. Il lit les valeurs de l'indicateur "CASH OSCILLATOR" pour repérer les points d'inflexion (lorsque le momentum change de direction). Il transmet un signal d'achat (`BUY`) lorsque l'oscillateur monte et un signal de vente (`SELL`) lorsqu'il descend. Ce module est crucial pour détecter les opportunités au début des mouvements de prix.

    3. Arrows.mqh
    Ce module est le déclencheur principal. Il est conçu pour lire les signaux (flèches) d'un indicateur de flèches. Son rôle est de lancer le processus de validation dans le module de décision. C'est le signal initial qui active la logique d'alignement pour chercher des confirmations. Tu peux le remplacer par l'indicateur de ton choix en lisant ses tampons de données (buffers) pour identifier les signaux de flèches.


# Le Module `Decision.mqh : Le Cerveau

Ce module est le véritable moteur de décision du système. Il reçoit les signaux de tous les autres modules et les analyse pour déterminer si une opportunité de trading existe.

    - Alignement des signaux : Decision.mqh attend que les signaux de tous les modules concordent. Il ne prendra une position que si un nombre minimum de modules (configuré par `MinSignauxAlignes`) sont alignés dans la même direction (par exemple, tous les trois modules indiquent `BUY`).

    - Gestion des risques : Une fois qu'une position est ouverte, le module gère la prise de profit (TP) et le stop loss (SL) selon les paramètres que tu as définis (`TP_Points` et `SL_Points`). Il crée même des rectangles visuels sur le graphique pour te montrer les zones de risque et de gain.

    - Logique de blocage : Pour éviter les sur-trades et les faux signaux, ce module implémente une logique de blocage. Une fois qu'il a ouvert une position (par exemple un achat), il bloque toute nouvelle position dans la même direction jusqu'à ce qu'un signal contraire (une flèche de vente) soit détecté, réinitialisant ainsi le cycle de trading.
