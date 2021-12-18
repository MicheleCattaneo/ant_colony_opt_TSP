# Ant Colony Optimization

This program solves the Travelling Salesman Problem (TSP) using the classical Ant Colony Optimization (ACO) with some additional features.

When the random number <img src="https://render.githubusercontent.com/render/math?math=q \sim U(0,1) > Q_0"> , the city that maximises <img src="https://render.githubusercontent.com/render/math?math=\left\{%5B\tau(r, u)%5D \cdot %5B \eta(r, u)%5D^{\beta}\right\}">

can be ignored when a city is picked according to the probability function  <img src="https://render.githubusercontent.com/render/math?math=S \sim \frac{[\tau(r, u)] \cdot[\eta(r, u)]^{\beta}}{\sum_{u \in J_{k}(r)}[\tau(r, u)] \cdot[\eta(r, u)]^{\beta}}">.

In the current implementation this is however not done, as the results seemed to be worse when ignoring the best city.

When all the ants have completed a tour (at the end of each iteration), one of them is picked so that their current tour is improved with a local search. The "lucky ant" is alternating every iteration between being the ant with the best tour length in the current iteration and a randomly chosen ant. This is because we can not exclude that an ant that has a current longer path is located in the area of the global minimum (just not reached yet), while the best ant is just located in a local minimum area.

The local search is also alternating between a **2OPT** and a **2.5OPT**. This was simply trial and error, and it seemed to be a good solution.

The typical global update rule is as follows, with <img src="https://render.githubusercontent.com/render/math?math=\alpha = 0.1 ">:

<img src="https://render.githubusercontent.com/render/math?math=
\tau(r, s) \leftarrow(1-\alpha) \cdot \tau(r, s)%2B \alpha \cdot \Delta \tau(r, s) \\
">

In this implementation, however, the whole $\tau(r, s)$ term is set to 0 and not only <img src="https://render.githubusercontent.com/render/math?math=\Delta \tau(r, s)$, when the edge $(r,s)"> is not on the global best tour. This results in the global update rule to be only applied on those edges that belong to the global best tour, and not the whole pheromone matrix.

The local update rule is the usual:

<img src="https://render.githubusercontent.com/render/math?math=\tau(r, s) \leftarrow(1-\rho) \cdot \tau(r, s)+\rho \cdot \tau_0">

However, $\rho$ it is not set to $0.1$ as the usual recommended value; it is set to <img src="https://render.githubusercontent.com/render/math?math=0.3">. This is because I wanted that edge $(r,s)$ would tend a bit more towards the initial <img src="https://render.githubusercontent.com/render/math?math=\tau_0"> value whenever is is used locally, and can then potentially be confirmed with a stronger pheromone with the global update, if the edge is part of the best global tour.

With a time limit of 3 minutes and without farming a lucky seed, the following result could be achieved:

| Problem: | Best Known: | Result: | Error: |
| -------- | ----------- | ------- | ------ |
| ch130    | 6110        | 6110    | 0%     |
| d198     | 15780       | 15780   | 0%     |
| eil76    | 538         | 538     | 0%     |
| fl1577   | 22249       | 22613   | 1.64%  |
| kroa100  | 21282       | 21282   | 0%     |
| lin318   | 42029       | 42143   | 0.27%  |
| pcb442   | 50778       | 51023   | 0.48%  |
| pr439    | 107217      | 107217  | 0%     |
| rat783   | 8806        | 8895    | 1.01%  |
| u1060    | 224094      | 227128  | 1.35%  |

The mean error is <img src="https://render.githubusercontent.com/render/math?math=0.4761">. 

Note that the parameters were fixed and some problems work better with certain set of parameters. This was a try to get an overall good result.

Following are some examples of results:

- PR439

<img src="./imgs/pr439.png">

- RAT783

<img src="./imgs/rat783.png">

- U1060

<img src="./imgs/u1060.png">

