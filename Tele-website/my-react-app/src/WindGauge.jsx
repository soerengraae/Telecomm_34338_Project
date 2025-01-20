import React from 'react';
import { Gauge, gaugeClasses } from '@mui/x-charts/Gauge';
import '@mui/material/styles';
import '@mui/x-charts/Gauge';

function WindGauge({ windSpeed }) {
  const minWind = 0;
  const maxWind = 20; // Assuming 20 m/s as the maximum wind speed for the gauge

  // Calculate percent value and clamp it between 0 and 100
  const percentValue = Math.max(0, Math.min(100, (windSpeed / maxWind) * 100));

  // Function to get a color from green (calm) to red (strong wind) based on wind speed
  const getGradientColor = (speed) => {
    const ratio = speed / maxWind;
    const green = Math.round(255 * (1 - ratio)); // More green when calm
    const red = Math.round(255 * ratio); // More red when windy
    return `rgb(${red}, ${green}, 0)`;
  };

  return (
    <div className="box">
      <h3>Vindhastighed {windSpeed} m/s</h3>

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
              fill: getGradientColor(windSpeed), // Dynamic arc color
            },
            [`& .${gaugeClasses.referenceArc}`]: {
              fill: theme.palette.text.disabled,
            },
          })}
        />
        
        {/* Overlay div to display the wind speed value in the center */}
        <div
          style={{
            position: 'absolute',
            top: '50%',
            left: '50%',
            transform: 'translate(-50%, -50%)',
            fontSize: '20px',
            fontWeight: 'bold',
          }}
        >
          {windSpeed} m/s
        </div>
      </div>
    </div>
  );
}

export default WindGauge;
