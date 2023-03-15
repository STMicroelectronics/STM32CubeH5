
:: flag to switch between OEMiROT and OEMuROT
set oemurot_enable=0

if %oemurot_enable% == 1 (
set project=OEMuROT
set bootpath=STiROT_OEMuROT)

if %oemurot_enable% == 0 (
set project=OEMiROT
set bootpath=OEMiROT)