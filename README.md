# HetSoCOpt
Simulator for heterogeneous SoC optimization (HetSoCOpt): \
\
Modeling and simulation of system bus and memory collision in heterogeneous SoCs, Jooho Wang, Yunkyu Gim, Sungkyung Park, and Chester Sungchung Park, IEEE Access, to be published.\
\
This SoC Simulator (HetSoCopt) can help to optimize design parameters and hence minimize the number of collisions. In order to search the optimal design parameter combination which meets the user requirement, the proposed simulator has some knobs: partitioning between software and hardware, scheduling the operations in the system, and memory merging, all of which can be adjusted to predict collisions and search the optimal heterogeneous SoC architecture. In addition, design parameters can be adjusted sequentially to cover all design options and estimate the predicted performance for each option. The proposed simulator can predict the performance of heterogeneous systems on chips with under 5% error for all the candidate architectures for OMP while taking the system bus and memory conflicts into account. Moreover, the optimized heterogeneous SoC architecture for the OMP algorithm improves performance by up to 32% compared with the conventional CAG-based approach. The proposed simulator is verified that the proposed performance estimation algorithm is generally applicable to estimate the performance of any heterogeneous SoC architecture. For example, the estimation error is measured to be no more than 5.9% for the convolutional layers of AlexNet and no more than 5.6% for the LDPC-coded MIMO-OFDM.

Requirements:
=============
We *highly* recommend that users use the `Visual Studio 2017 (x86)`,
as it has all the required dependencies installed and the environment prepared.

Create an empty project:
=============
You can refer to [this](https://drive.google.com/file/d/1YOwjZA74C_UD5CA0cvvzqUBCzprOdcsw/view?usp=sharing) and create an empty project easily.

Build:
======
After setting the new project:
Build => Solution Build (Ctrl + Shift + B)

Run:
======
After you build HetSoCOpt, you can use the provided programs in the orthogonal matching pursuit (OMP) algorithm for channel estimation (LTE 5MHz) to test HetSoCOpt:\

Debug => Start without debugging (Ctrl + F5) *or* Debug => Start debugging (F5)

HetSoCOpt will generate a text file during its execution. Files you might be interested are `Files0~6.txt` which profiles the estimated performance of heterogeneous SoC models.

Jooho Wang and Prof. Chester Sungchung Park.\
If you have any questions, please send an email to joohowang@konkuk.ac.kr\
Original release: Konkuk University, 2022\
