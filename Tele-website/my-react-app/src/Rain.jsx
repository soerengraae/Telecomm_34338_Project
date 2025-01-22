import React from 'react';

/**
 * @file Rain.jsx
 * @brief React component to display the rain status.
 * @details Displays whether it is currently raining or not, with a dynamic color and icon based on the status.
 * @param {boolean} isRaining - Indicates if it is raining (`true`) or not (`false`).
 * @author Janus Meier
 */

/**
 * @function Rain
 * @brief Renders a box displaying the rain status.
 * @param {Object} props - The properties passed to the component.
 * @param {boolean} props.isRaining - Indicates if it is raining (`true`) or not (`false`).
 * @returns {JSX.Element} The rendered Rain component.
 */
function Rain({ isRaining }) {
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
        textAlign: 'center',
      }}
    >
      <h3>Regnstatus</h3>
      <p
        style={{
          fontSize: '24px',
          fontWeight: 'bold',
          color: isRaining ? 'blue' : 'gray', // Blue for rain, gray for no rain
        }}
      >
        {isRaining ? 'Det regner 🌧️' : 'Ingen regn ☀️'}
      </p>
    </div>
  );
}

export default Rain;
