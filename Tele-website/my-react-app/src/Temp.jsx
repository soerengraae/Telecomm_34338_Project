import React from 'react';
import { Gauge, gaugeClasses } from '@mui/x-charts/Gauge';
import '@mui/material/styles';
import '@mui/x-charts/Gauge';

function TempGauge({ temperature }) {
  const minTemp = -20;
  const maxTemp = 40;

  // Calculate percent value and clamp it between 0 and 100
  const percentValue = Math.max(0, Math.min(100, ((temperature - minTemp) / (maxTemp - minTemp)) * 100));

  // Function to get a color between blue and red based on temperature
  const getGradientColor = (temp) => {
    const ratio = (temp - minTemp) / (maxTemp - minTemp); // Normalize to range [0, 1]
    const blue = Math.round(255 * (1 - ratio)); // More blue when colder
    const red = Math.round(255 * ratio); // More red when hotter
    return `rgb(${red}, 0, ${blue})`;
  };

  return (
    <>
      {/* Box with drop shadow */}
      <div className="box"
        style={{
          width: '250px',
          padding: '20px',
          borderRadius: '15px',
          backgroundColor: 'white',
          boxShadow: '0 4px 10px rgba(0, 0, 0, 0.2)', // Drop shadow
          margin: '20px auto', // Center horizontally
        }}
      >
        <h3>Temperatur {temperature} °C</h3>

        <div style={{ width: '200px', height: '200px', position: 'relative', margin: '0 auto' }}>
          <Gauge
            value={percentValue}
            startAngle={-105}
            endAngle={105}
            innerRadius="80%"
            outerRadius="100%"
            needleColor="black"
            cornerRadius="50%"
            sx={(theme) => ({
              [`& .${gaugeClasses.valueText}`]: {
                fontSize: 40,
                display: 'none',
              },
              [`& .${gaugeClasses.valueArc}`]: {
                fill: getGradientColor(temperature), // Dynamic arc color
              },
              [`& .${gaugeClasses.referenceArc}`]: {
                fill: theme.palette.text.disabled,
              },
            })}
          />
          {/* Overlay to hide the percentage */}
          {/* <div
            style={{
              position: 'absolute',
              top: '65%',
              left: '50%',
              transform: 'translate(-50%, -50%)',
              backgroundColor: 'white',
              width: '120px',
              height: '60px',
            }}
          /> */}

          {/* Label to show temperature */}
          <div
            style={{
              position: 'absolute',
              top: '60%',
              left: '50%',
              transform: 'translate(-50%, -50%)',
              fontSize: '20px',
              fontWeight: 'bold',
            }}
          >
            {temperature} °C
          </div>
        </div>
      </div>
    </>
  );
}

export default TempGauge;
