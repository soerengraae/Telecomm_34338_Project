import React from 'react';
import { Gauge, gaugeClasses } from '@mui/x-charts/Gauge';
import '@mui/material/styles';
import '@mui/x-charts/Gauge';

/**
 * @file HumidityGauge.jsx
 * @brief React component to display a humidity gauge.
 * @details The gauge visualizes the humidity as a percentage using a dynamic color arc.
 * @param {number} humidity - The current humidity value to be displayed.
 * @author Janus Meier
 */

/**
 * @function HumidityGauge
 * @brief Renders a gauge displaying the current humidity.
 * @param {Object} props - The properties passed to the component.
 * @param {number} props.humidity - The current humidity value (in percentage).
 * @returns {JSX.Element} The rendered HumidityGauge component.
 */
function HumidityGauge({ humidity }) {
  /**
   * @var minHumidity
   * @brief The minimum humidity value for the gauge.
   * @type {number}
   */
  const minHumidity = 0;

  /**
   * @var maxHumidity
   * @brief The maximum humidity value for the gauge.
   * @type {number}
   */
  const maxHumidity = 100;

  /**
   * @var percentValue
   * @brief The normalized percentage value for the gauge based on the humidity.
   * @type {number}
   */
  const percentValue = Math.max(0, Math.min(100, (humidity / maxHumidity) * 100));

  /**
   * @function getGradientColor
   * @brief Calculates the color of the gauge arc based on the humidity value.
   * @param {number} hum - The current humidity value.
   * @returns {string} The RGB color string representing the humidity level.
   */
  const getGradientColor = (hum) => {
    const ratio = hum / maxHumidity; // Normalize to range [0, 1]
    const blue = Math.round(255 * ratio); // Darker blue when more humid
    return `rgb(0, 0, ${blue})`;
  };

  return (
    <div
      className="box"
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
