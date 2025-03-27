# RNA_Splicing
Models and analyses related to RNA splicing performed in Chris Burge's lab at MIT. I am in the process of uploading everything.

Abbreviations: 3SS = 3' Splice Site, PPT = Polypyrimidine Tract

Most scripts require intronic sequence parses near the splice site vs. deep intronic (control).

HMM_PPT_Model is a two-state (PPT vs. Random) hidden-Markov model of the 3' splice site.
Heuristic_PPT_Model is a rule-based model to identify PPTs 
MLP_NN_SF3B1 is a multi-layer perceptron neural network to predict splice sites that could be sensitive to cryptic splicing upon SF3B1 mutation.
