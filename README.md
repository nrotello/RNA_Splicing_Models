# RNA_Splicing
RNA splicing models and analyses performed at MIT. 

Abbreviations: 3SS = 3' Splice Site, PPT = Polypyrimidine Tract

HMM_PPT_Model is a two-state (PPT vs. Random) hidden-Markov model of the 3' splice site. I have built three state models in the past (Pre-PPT, PPT, and Post-PPT).

Heuristic_PPT_Model is a rule-based model to identify PPTs.

MLP_NN_SF3B1 is a multi-layer perceptron neural network to predict splice sites that could be sensitive to cryptic splicing upon SF3B1 mutation.

CAM_SS_Model is a splice site scoring model that accounts for long-range nucleotide dependencies. Original idea from Chris Burge, MIT.

