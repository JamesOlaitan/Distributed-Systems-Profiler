import http from 'k6/http';
import { sleep } from 'k6';

export const options = {
  scenarios: {
    constant_rate: {
      executor: 'constant-arrival-rate',
      rate: __ENV.RATE ? parseInt(__ENV.RATE) : 1000, // iterations per timeUnit
      timeUnit: '1s',
      duration: __ENV.DURATION || '60s',
      preAllocatedVUs: __ENV.VUS ? parseInt(__ENV.VUS) : 200,
      maxVUs: __ENV.MAX_VUS ? parseInt(__ENV.MAX_VUS) : 400,
    },
  },
};

export default function () {
  const url = __ENV.URL || 'http://localhost:8000/data';
  http.get(url);
}

