import React, { useState, useEffect } from 'react';
import './API.css';
import TempGauge from './Temp';
import HumidityGauge from './HumidityGauge';
import Rain from './Rain';
import WindGauge from './WindGauge';

function API() {
  const [latestData, setLatestData] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const url = 'https://api.thingspeak.com/channels/2810374/feeds.json?results=2'; 
    const url1 = 'https://api.thingspeak.com/channels/2808342/feeds.json?results=2';

    fetch(url)
      .then((response) => response.json())
      .then((data) => {
        if (data.feeds && data.feeds.length > 0) {
          setLatestData(data.feeds[0]); // Save the latest data point
        }
      })
      .catch((error) => console.error('Error fetching data:', error))
      .finally(() => setLoading(false));
  }, []);

  // Extract temperature, humidity, rain status, and wind speed
  const temperature = latestData ? parseFloat(latestData.field2) : null;
  const humidity = latestData ? parseFloat(latestData.field3) : null;
  const isRaining = latestData ? parseInt(latestData.field6) === 1 : false; // Assume 1 means rain, 0 means no rain
  const windSpeed = latestData ? parseFloat(latestData.field4) : null;

  return (
    <div className="page-container">
      <div>
        {loading ? (
          <p>Indlæser data...</p>
        ) : (
          <div className="gauge-container">
            <TempGauge temperature={temperature} />
            <HumidityGauge humidity={humidity} />
            <Rain isRaining={isRaining} />
            <WindGauge windSpeed={windSpeed} />
          </div>
        )}
      </div>
    </div>
  );
}

export default API;
