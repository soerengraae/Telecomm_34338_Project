import React from 'react';

function Rain({ isRaining }) {
  return (
    <div className="box"
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
          color: isRaining ? 'blue' : 'gray',
        }}
      >
        {isRaining ? 'Det regner 🌧️' : 'Ingen regn ☀️'}
      </p>
    </div>
  );
}

export default Rain;
