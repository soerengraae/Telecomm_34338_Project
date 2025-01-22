import React from 'react';
import { Gauge, gaugeClasses } from '@mui/x-charts/Gauge';
import '@mui/material/styles';
import '@mui/x-charts/Gauge';

/**
 * @file WindGauge.jsx
 * @brief React component to display a wind speed gauge.
 * @details The gauge visualizes the wind speed in meters per second (m/s) using a dynamic color arc that transitions from green (calm) to red (strong wind).
 * @param {number} windSpeed - The current wind speed to be displayed in m/s.
 * @author Janus Meier
 */

/**
 * @function WindGauge
 * @brief Renders a gauge displaying the current wind speed.
 * @param {Object} props - The properties passed to the component.
 * @param {number} props.windSpeed - The current wind speed (in m/s).
 * @returns {JSX.Element} The rendered WindGauge component.
 */
function WindGauge({ windSpeed }) {
  /**
   * @var minWind
   * @brief The minimum wind speed value for the gauge.
   * @type {number}
   */
  const minWind = 0;

  /**
   * @var maxWind
   * @brief The maximum wind speed value for the gauge.
   * @type {number}
   */
  const maxWind = 20; // Assuming 20 m/s as the maximum wind speed for the gauge

  /**
   * @var percentValue
   * @brief The normalized percentage value for the gauge based on the wind speed.
   * @type {number}
   */
  const percentValue = Math.max(0, Math.min(100, (windSpeed / maxWind) * 100));

  /**
   * @function getGradientColor
   * @brief Calculates the color of the gauge arc based on the wind speed value.
   * @param {number} speed - The current wind speed value.
   * @returns {string} The RGB color string representing the wind speed level.
   */
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
