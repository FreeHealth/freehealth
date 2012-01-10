function framinghamCalcul()
{
    freemedforms.forms.namespaceInUse = "Subs::Cardiology::FraminghamScore::FraminghamGroup";
    var age = freemedforms.patient.yearsOld;
    var total_cholesterol = freemedforms.forms.item("Cholesterol::TotalCholesterol").currentValue;
    var HDL_cholesterol = freemedforms.forms.item("Cholesterol::HDLCholesterol").currentValue;
    var tobacco = freemedforms.forms.item("Tobacco").checked;
    var bloodpressure = freemedforms.forms.item("BloodPressure::SystolicBloodPressure").currentValue;
    var bloodpressure_treated = freemedforms.forms.item("BloodPressure::TreatedBloodPressure").checked;
    var score = 0;
    var riskscore;
    
    var lang = freemedforms.forms.currentLanguage;
    var enabledforms = freemedforms.forms.item("FraminghamGroup").enabled;
    
   //Verification patient age
    if (age < 20 || age >= 80 ) 
    {
        
        if (lang == "xx")
            {riskscore ="Patient age not evaluated in the Framingham Score "; return riskscore;}
        else if (lang=="fr")
            {riskscore ="Tranche d'\u00E2ge non \u00E9valu\u00E9e par le score de Framingham"; return riskscore;}
        }
       
    
    
     if (freemedforms.patient.isFemale){
    
    //Age
    
        if (age>=20 && age < 35) score -=7; 
        else if (age >= 35 && age < 40) score -=3; 
        else if (age >=40 && age < 44) score -= 0; 
        else if (age >=45 && age < 50) score += 3; 
        else if (age >=50 && age < 55) score += 6; 
        else if (age >=55 && age < 60) score += 8; 
        else if (age >=60 && age < 65) score += 10; 
        else if (age >=65 && age < 70) score += 12; 
        else if (age >=70 && age < 75) score += 14; 
        else   score += 16;  
        
    
    //total_cholesterol
    if (age >= 20 && age < 40){
   
        if (total_cholesterol < 160) score -= 0; 
        else if (total_cholesterol >= 160 && total_cholesterol < 200) score += 4; 
        else if (total_cholesterol >= 200 && total_cholesterol < 240) score += 8; 
        else if (total_cholesterol >= 240 && total_cholesterol < 280) score += 11; 
        else  score += 13; 
        }        
        
    
    else if (age >= 40 && age < 50){
    
        if (total_cholesterol <= 159) score -= 0; 
        else if (total_cholesterol >= 160 && total_cholesterol <= 199) score += 3; 
        else if (total_cholesterol >= 200 && total_cholesterol <= 239) score += 6; 
        else if (total_cholesterol >= 240 && total_cholesterol <= 279) score += 8; 
        else  score += 10; 
        }        
        
   else if (age >= 50 && age < 60){
    
        if (total_cholesterol < 160) score -= 0; 
        else if (total_cholesterol >= 160 && total_cholesterol < 200) score += 2; 
        else if (total_cholesterol >= 200 && total_cholesterol < 240) score += 4; 
        else if (total_cholesterol >= 240 && total_cholesterol < 280) score += 5; 
        else   score += 7;  
        }        
        
                     
    else  if (age >= 60 && age < 70){
   
        if (total_cholesterol < 160) score -= 0; 
        else if (total_cholesterol >= 160 && total_cholesterol < 200) score += 1; 
        else if (total_cholesterol >= 200 && total_cholesterol < 240) score += 2; 
        else if (total_cholesterol >= 240 && total_cholesterol < 280) score += 3; 
        else   score += 4; 
        }        
        
        
    else {  
    
        if (total_cholesterol < 160) score -= 0; 
        else if (total_cholesterol >= 160 && total_cholesterol < 200) score += 1; 
        else if (total_cholesterol >= 200 && total_cholesterol < 240) score += 1; 
        else if (total_cholesterol >= 240 && total_cholesterol < 280) score += 2; 
        else   score += 2; 
        }        
        
    
  //tobacco
  if (tobacco){
   
       
        if (age >= 20 && age < 40) score +=9; 
        else if (age >=40 && age < 50) score += 7; 
        else if (age >=50 && age < 60) score += 4; 
        else if (age >=60 && age < 70) score += 2; 
        else  score += 1; 
          }
        
    
    //HDL Cholesterol
    
        if (HDL_cholesterol >=60) score += -1; 
        else if (HDL_cholesterol >=50 && HDL_cholesterol < 60) score+= 0; 
        else if (HDL_cholesterol >=40 && HDL_cholesterol < 50) score+= 1; 
        else   score += 2;  
    
    
    // Systolic blood pressure
    if (!bloodpressure_treated){
   
        if (bloodpressure < 120) score += 0; 
        else if (bloodpressure >=120 && bloodpressure < 130)  score += 1; 
        else if (bloodpressure >=130 && bloodpressure < 140)  score += 2; 
         else if (bloodpressure >=140 && bloodpressure < 160)  score += 3; 
         else    score += 4; 
    }
    
    else {
    
        if (bloodpressure < 120) score += 0; 
        else if (bloodpressure >=120 && bloodpressure < 130)  score += 3; 
        else if (bloodpressure >=130 && bloodpressure < 140)  score += 4; 
        else  if (bloodpressure >=140 && bloodpressure < 160) score += 5; 
         else  score += 6; 
         }
    
 // Calcul of risk score  
   
    if (score <9) riskscore = "< 1%"; 
    else if (score >=9 && score <= 12) riskscore = "1%"; 
    else if (score >=13 && score <= 14) riskscore = "2%";   
    else if (score == 15) riskscore = "3%"; 
     else if (score == 16) riskscore = "4%"; 
      else if (score == 17) riskscore = "5%"; 
       else if (score == 18) riskscore = "6%"; 
       else if (score == 19) riskscore = "8%"; 
       else if (score == 20) riskscore = "11%"; 
       else if (score == 21) riskscore = "14%"; 
       else  if (score == 22) riskscore = "17%"; 
       else  if (score == 23) riskscore = "22%"; 
        else if (score == 24) riskscore = "27%"; 
       else   riskscore = "\u226530%"; 
      

return riskscore;
    
}

else {
    
 //Age
    
        if (age < 35) score -=9; 
        else if (age >=35 && age < 40) score -=4; 
        else if (age >=40 && age < 45) score -= 0; 
        else if (age >=45 && age < 50) score += 3; 
        else if (age >=50 && age < 55) score += 6; 
        else if (age >=55 && age < 60) score += 8; 
        else if (age >=60 && age < 65) score += 10; 
        else if (age >=65 && age < 70) score += 11; 
        else if (age >=70 && age < 75) score += 12; 
        else  score += 13; 
    }
    
    //total_cholesterol
    if (age >=20 && age <40){
    
        if (total_cholesterol < 160) score -= 0; 
        else if (total_cholesterol >=160 && total_cholesterol < 200) score += 4; 
        else if (total_cholesterol >=200 && total_cholesterol < 240) score += 7; 
        else if (total_cholesterol >=240 && total_cholesterol < 280) score += 9; 
        else  score += 11; 
        }        
        
    
    else if (age >=40 && age <50){
   
        if (total_cholesterol < 160) score -= 0; 
        else if (total_cholesterol >=160 && total_cholesterol < 200) score += 3; 
        else if (total_cholesterol >=200 && total_cholesterol < 240) score += 5; 
        else if (total_cholesterol >=240 && total_cholesterol < 280) score += 6; 
        else   score += 8; 
        }        
        
   else if (age > 49 && age <60){
    
        if (total_cholesterol < 160) score -= 0; 
        else if (total_cholesterol >=160 && total_cholesterol < 200) score += 2; 
        else if (total_cholesterol >=200 && total_cholesterol < 240) score += 3; 
        else if (total_cholesterol >=240 && total_cholesterol < 280) score += 4; 
        else   score += 5; 
        }        
        
        
               
       else  if (age >=60 && age <70){
    
        if (total_cholesterol < 160) score -= 0; 
        else if (total_cholesterol >=160 && total_cholesterol < 200) score += 1; 
        else if (total_cholesterol >=200 && total_cholesterol < 240) score += 1; 
        else if (total_cholesterol >=240 && total_cholesterol > 280) score += 2; 
        else   score += 3; 
        }        
        
        
       else   { 
   
        if (total_cholesterol < 160) score -= 0; 
        else if (total_cholesterol >=160 && total_cholesterol < 200) score += 0; 
        else if (total_cholesterol >=200 && total_cholesterol < 240) score += 0; 
        else if (total_cholesterol >=240 && total_cholesterol < 280) score += 1; 
        else   score += 1; 
        }        
        
    
  //tobacco
  if (tobacco){
    
       
        if (age >= 20 && age < 40) score +=8; 
        else if (age >=40 && age < 50) score += 5; 
        else if (age >=50 && age < 60) score += 3; 
        else if (age >=60 && age < 70) score += 1; 
        else   score += 1; 
          }
        
    
    //HDL Cholesterol
    
        if (HDL_cholesterol >=60) score += -1; 
        else if (HDL_cholesterol >=50 && HDL_cholesterol < 60) score+= 0; 
        else if (HDL_cholesterol >=40 && HDL_cholesterol < 50) score+= 1; 
        else   score += 2;    
    
    
    // Systolic blood pressure
    if (!bloodpressure_treated){
    
        if (bloodpressure < 120) score += 0; 
        else if (bloodpressure >=120 && bloodpressure < 130)  score += 1; 
        else if (bloodpressure >=130 && bloodpressure < 140)  score += 2; 
        else  if (bloodpressure >=140 && bloodpressure < 160) score += 3; 
         else   score += 4; 
    }
    
    else {
   
        if (bloodpressure < 120) score += 0; 
        else if (bloodpressure >=120 && bloodpressure < 130)  score += 3; 
        else if (bloodpressure >=130 && bloodpressure < 140)  score += 4; 
        else  if (bloodpressure >=140 && bloodpressure < 160) score += 5; 
         else   score += 6; 
         }
      
      

    if (score <0) riskscore =" < 1% ";
    else if (score>=0 && score <= 4) riskscore="1%";
    else if (score>=5 && score <= 6) riskscore="2%";
    else if (score==7) riskscore="3%";
    else if (score==8) riskscore="4%";
    else if (score==9) riskscore="5%";
    else if (score ==10)  riskscore = "6%"; 
    else if (score ==11) riskscore = "8%"; 
    else if (score ==12) riskscore = "10%"; 
    else if (score ==13) riskscore = "12%"; 
    else if (score ==14) riskscore = "16%"; 
    else if (score ==15) riskscore = "20%"; 
    else if (score ==16) riskscore = "25%"; 
    else   riskscore = "\u226530%"; 
             

//DEBUG
// riskscore = 23
//+" "+
//    total_cholesterol
//+" "+
//     HDL_cholesterol 
//+" "+
//    tobacco 
//+" "+
//   bloodpressure 
//+" "+
//     bloodpressure_treated
//+" "+
//    score 
//;

    
return riskscore;

}
