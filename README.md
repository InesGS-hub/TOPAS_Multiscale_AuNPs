# TOPAS_Multiscale_AuNPs

This work is divided in 3 Stages: Macroscopic (irradiation); Microscopic (cell with AuNPs); and Microscopic (nucleus with DNA). The aim of this work is to study the dose enhancement effect and radiosensitization effect of AuNPs.

Geometry setup for Stage 2 (cell geometry):
<p align="center">
<img width="776" alt="Captura de ecrã 2024-06-28, às 11 27 41" src="https://github.com/InesGS-hub/TOPAS_Multiscale_AuNPs/assets/174004445/80f3a521-4f94-4609-a9a6-7f6b478f0b0b">
</p>
_____________________________________________________________________________________________________

The TOPAS files are:

  - phase_1_macro.txt (main)
      - ScoreAll.txt
      - ScoreOut.txt
      - ScoreIn.txt
      - PhaseSpaceStage1.txt
      - plate24.txt
  
  - phase_2_micro_22.3_22.3.txt
      - points_0_in.txt
      - points_0_out.txt
      - points_108mil_out.txt
      - points_114mil_out.txt
      - points_120mil_in.txt
      - points_120mil_out.txt
      - points_12mil_in.txt
      - points_6mil_in.txt
      - PhaseSpaceStage2.txt
      - BeamSourceStage2.txt

  - phase3_nuclearscale.txt or phase3_ClusterScore.txt
      - PhaseSpaceStage3.txt
___________________________________________________________________________________________________________________
Auxiliar Files:
  - pruebaNanoOuro.txt (tunning of physics modules - for explicit AuNPs CH is not the most apropriate physics module)

<p align="center">
  <img width="506" alt="image" src="https://github.com/InesGS-hub/TOPAS_Multiscale_AuNPs/assets/174004445/af0afb9e-111b-46ee-a17d-99e14eefd59c">
</p>
   
  - dividir_PHSP.py (python code to divide the PHSP from stage 2, in smaller PHSPs with 1000 histories)
  - TsDistributedComponents extension for stage 2 (position of AuNPs and geometry)
  - TsCellNucleus extension for stage 2 (geometry of cell without nucleus)
<p align="center">
<img width="355" alt="image" src="https://github.com/InesGS-hub/TOPAS_Multiscale_AuNPs/assets/174004445/25fd821f-6015-4b9e-8e3d-dc5339c5427e">
</p>

    
