import React from 'react';
import { Gauge, gaugeClasses } from '@mui/x-charts/Gauge';
import '@mui/material/styles';
import '@mui/x-charts/Gauge';

/**
 * @file Temp.jsx
 * @brief React component to display a temperature gauge.
 * @details The gauge visualizes the temperature in degrees Celsius using a dynamic color arc.
 * @param {number} temperature - The current temperature value to be displayed.
 * @author Your Name
 */

/**
 * @function TempGauge
 * @brief Renders a gauge displaying the current temperature.
 * @param {Object} props - The properties passed to the component.
 * @param {number} props.temperature - The current temperature value (in °C).
 * @returns {JSX.Element} The rendered TempGauge component.
 */
function TempGauge({ temperature }) {
  /**
   * @var minTemp
   * @brief The minimum temperature value for the gauge.
   * @type {number}
   */
  const minTemp = -20;

  /**
   * @var maxTemp
   * @brief The maximum temperature value for the gauge.
   * @type {number}
   */
  const maxTemp = 40;

  /**
   * @var percentValue
   * @brief The normalized percentage value for the gauge based on the temperature.
   * @type {number}
   */
  const percentValue = Math.max(0, Math.min(100, ((temperature - minTemp) / (maxTemp - minTemp)) * 100));

  /**
   * @function getGradientColor
   * @brief Calculates the color of the gauge arc based on the temperature value.
   * @param {number} temp - The current temperature value.
   * @returns {string} The RGB color string representing the temperature level.
   */
  const getGradientColor = (temp) => {
    const ratio = (temp - minTemp) / (maxTemp - minTemp); // Normalize to range [0, 1]
    const blue = Math.round(255 * (1 - ratio)); // More blue when colder
    const red = Math.round(255 * ratio); // More red when hotter
    return `rgb(${red}, 0, ${blue})`;
  };

  return (
    <>
      {/* Box with drop shadow */}
      <div
        className="box"
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
                display: 'none', // Hide default value text from the Gauge component
              },
              [`& .${gaugeClasses.valueArc}`]: {
                fill: getGradientColor(temperature), // Dynamic arc color
              },
              [`& .${gaugeClasses.referenceArc}`]: {
                fill: theme.palette.text.disabled,
              },
            })}
          />

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
