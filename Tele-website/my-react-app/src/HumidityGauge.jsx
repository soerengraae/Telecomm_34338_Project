import React from 'react';
import { Gauge, gaugeClasses } from '@mui/x-charts/Gauge';
import '@mui/material/styles';
import '@mui/x-charts/Gauge';

function HumidityGauge({ humidity }) {
  const minHumidity = 0;
  const maxHumidity = 100;

  // Calculate percent value and clamp it between 0 and 100
  const percentValue = Math.max(0, Math.min(100, (humidity / maxHumidity) * 100));

  // Function to get a color from light blue to dark blue based on humidity
  const getGradientColor = (hum) => {
    const ratio = hum / maxHumidity; // Normalize to range [0, 1]
    const blue = Math.round(255 * ratio); // Darker blue when more humid
    return `rgb(0, 0, ${blue})`;
  };

  return (
    <div className="box"
      style={{
        width: '250px',
        padding: '20px',
        borderRadius: '15px',
        backgroundColor: 'white',
        boxShadow: '0 4px 10px rgba(0, 0, 0, 0.2)',
        margin: '20px auto',
      }}
    >
      <h3 className="humidity-gauge-heading">Fugtighed {humidity} %</h3>

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
              display: 'none', // Hide default value text from the Gauge component
            },
            [`& .${gaugeClasses.valueArc}`]: {
              fill: getGradientColor(humidity), // Dynamic arc color
            },
            [`& .${gaugeClasses.referenceArc}`]: {
              fill: theme.palette.text.disabled,
            },
          })}
        />
        
        {/* Overlay div to display the humidity value in the center */}
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
          {humidity} %
        </div>
      </div>
    </div>
  );
}

export default HumidityGauge;
