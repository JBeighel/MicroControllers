[aColNames, aDataValues] = fLoadCSVData(strcat("C:\\Users\\jbeighel\\Dropbox\\Code\\MicroControllers\\Projects\\DriverWork\\", "DataLog2.csv"), 1, 1);

aIndexes = 1:size(aDataValues, 2);

clear("aXData");
clear("aYData");
clear("aZData");

nXDataCtr = 1;
nYDataCtr = 1;
nZDataCtr = 1;
for (nRawCtr = 1:size(aDataValues, 2))
  if ((aDataValues(2, nRawCtr) < -150) || (aDataValues(2, nRawCtr) > 0)) 
    aXData(nXDataCtr) = aDataValues(2, nRawCtr);
    nXDataCtr += 1;
  endif
  
  if ((aDataValues(4, nRawCtr) < -150) || (aDataValues(4, nRawCtr) > 0)) 
    aYData(nYDataCtr) = aDataValues(4, nRawCtr);
    nYDataCtr += 1;
  endif
  
  if ((aDataValues(6, nRawCtr) < -150) || (aDataValues(6, nRawCtr) > 0)) 
    aZData(nZDataCtr) = aDataValues(6, nRawCtr);
    nZDataCtr += 1;
  endif
endfor

aXIntTrap = cumtrapz(aXData);
aYIntTrap = cumtrapz(aYData);
aZIntTrap = cumtrapz(aZData);

aXIntLeak = fLeakyIntegrator(aXData, 0.95);
aYIntLeak = fLeakyIntegrator(aYData, 0.95);
aZIntLeak = fLeakyIntegrator(aZData, 0.95);

nLen = min(size(aXData, 2), size(aYData, 2));
nLen = min(nLen, size(aYData, 2));
for (nRawCtr = 1:nLen)
  nAccum = aXIntLeak(nRawCtr) * aXIntLeak(nRawCtr);
  nAccum += aYIntLeak(nRawCtr) * aYIntLeak(nRawCtr);
  nAccum += aZIntLeak(nRawCtr) * aZIntLeak(nRawCtr);
  
  aVelocity(nRawCtr) = sqrt(nAccum);
endfor

PlotData = [];
#PlotData = AddLinePlotToChart("X Raw", aIndexes, aDataValues(2, :), PlotData);
#PlotData = AddLinePlotToChart("Y Raw", aIndexes, aDataValues(4, :), PlotData);
#PlotData = AddLinePlotToChart("Z Raw", aIndexes, aDataValues(6, :), PlotData);
PlotData = AddLinePlotToChart("X Filter", 1:size(aXData, 2), aXData, PlotData);
PlotData = AddLinePlotToChart("Y Filter", 1:size(aYData, 2), aYData, PlotData);
PlotData = AddLinePlotToChart("Z Filter", 1:size(aZData, 2), aZData, PlotData);
#PlotData = AddLinePlotToChart("X Trap", 1:size(aXData, 2), aXIntTrap, PlotData);
#PlotData = AddLinePlotToChart("Y Trap", 1:size(aYData, 2), aYIntTrap, PlotData);
#PlotData = AddLinePlotToChart("Z Trap", 1:size(aZData, 2), aZIntTrap, PlotData);
PlotData = AddLinePlotToChart("X Leak", 1:size(aXData, 2), aXIntLeak, PlotData);
PlotData = AddLinePlotToChart("Y Leak", 1:size(aYData, 2), aYIntLeak, PlotData);
PlotData = AddLinePlotToChart("Z Leak", 1:size(aZData, 2), aZIntLeak, PlotData);
PlotData = AddLinePlotToChart("Velocity", 1:size(aZData, 2), aVelocity, PlotData);

CreatePlot("Time", "Acceleration", PlotData, "DataLog", 1, size(aDataValues, 2), 'PowerPoint', true);

function [anIntegral] = fLeakyIntegrator(anDeriv, nLeakPct)
  nAccum = 0;
  
  for (nCtr = 1:size(anDeriv, 2))
    nAccum *= nLeakPct;
    nAccum += anDeriv(nCtr);
    
    anIntegral(nCtr) = nAccum;
  endfor
endfunction

function [nMagnitude] = fMagnitudeOf3dVector(nXCoord, nYCoord, nZCoord)
  nXCoord *= nXCoord;
  nXCoord += nYCoord * nYCoord;
  nXCoord += nZCoord * nZCoord;
  
  nMagnitude = sqrt(nXCoord);
endfunction