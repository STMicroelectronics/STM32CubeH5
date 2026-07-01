
#flag to switch between OEMiROT and OEMuROT
oemurot_enable=0

if [ "$oemurot_enable" == "1" ]; then
    project=OEMuROT
    bootpath=STiROT_OEMuROT
fi

if [ "$oemurot_enable" == "0" ]; then
    project=OEMiROT
    bootpath=OEMiROT
fi