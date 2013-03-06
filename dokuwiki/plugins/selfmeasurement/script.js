function selfmeasurement_clearAverages() {
    document.getElementById('systAverage').value = '';
    document.getElementById('diastAverage').value = '';
    document.getElementById('systNb').value = '';
    document.getElementById('diastNb').value = '';
}

function selfmeasurement_clear() {
    for(var i=0;i<18;i++) {
        document.getElementById('s'+(i+1)).value='';
        document.getElementById('d'+(i+1)).value='';
    }
    document.getElementById('param').value='0';
    selfmeasurement_clearAverages();
}

// Check all values of the form. If all values are correct, return true, false otherwise.
function selfmeasurement_checkValues() {
    for(i=0; i<18; i++) {
        if (document.getElementById('s'+(i+1)).value === '')
            return false;

        if (parseInt(document.getElementById('s'+(i+1)).value) === "NaN")
            return false;

        if (document.getElementById('d'+(i+1)).value === '')
            return false;

        if (parseInt(document.getElementById('d'+(i+1)).value) === "NaN")
            return false;
    }
    return true;
}

// Compute the systolic and diastolic averages. Populate the form with the values.
function selfmeasurement_computeAverages() {
    selfmeasurement_clearAverages();
    var syst=0;
    var nbSyst=0;
    var diast=0;
    var nbDiast=0;

    // Cycle throught the form
    for(i=0; i<18; i++) {
        if (document.getElementById('s'+(i+1)).value !== '') {
            syst += parseInt(document.getElementById('s'+(i+1)).value);
            nbSyst++;
        }

        if(document.getElementById('d'+(i+1)).value !== '') {
            diast += parseInt(document.getElementById('d'+(i+1)).value);
            nbDiast++;
        }
    }

    // Compute averages
    systAverage = syst/nbSyst;
    diastAverage = diast/nbDiast;
    if (!isNaN(systAverage)) {
        document.getElementById('systAverage').value = systAverage;
        document.getElementById('systNb').value = nbSyst;
    }

    if (!isNaN(diastAverage)) {
        document.getElementById('diastAverage').value = diastAverage;
        document.getElementById('diastNb').value = nbDiast;
    }
}
