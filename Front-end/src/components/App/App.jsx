import React, { useState, useEffect } from 'react';
import { v4 as uuidv4 } from 'uuid';
import 'materialize-css/dist/css/materialize.min.css';
import './App.css';
import BondForm from '../BondForm/BondForm';
import Info from '../Info/Info';
import { getData, storeData } from '../../helpers/localStorage';
import axios from 'axios';

const App = () => {
  const initialState = () => getData('data') || [];
  const [state, setState] = useState(initialState);
  const [data, setData] = useState({});

  useEffect(() => {
    storeData('data', state);
    // const date = state.map(obj => obj.date);
    const r0 = state.map(obj => obj.r0);
    const u = state.map(obj => obj.u);
    const d = state.map(obj => obj.d);
    const n = state.map(obj => obj.n);
    const q = state.map(obj => obj.q);

    const straight_bond_price = state.map(obj => obj.straight_bond_price);
    const callable_bond_price = state.map(obj => obj.callable_bond_price);

    let newData = { r0, u, d, n, q, straight_bond_price, callable_bond_price };
    setData(newData);
  }, [state]);

  const handleChange = val => {
    let r0 = parseFloat(val.r0);
    let u = parseFloat(val.u);
    let d = parseFloat(val.d);
    let n = parseInt(val.n);
    let q = parseFloat(val.q);
  
    axios.post('http://localhost:3001/calculate', {
      r0, u, d, n, q
    })
      .then(response => { 
      setState(prevState => {
            val.r0 = r0;
            val.u = u;
            val.d = d;
            val.n = n;
            val.q = q;

            val.straight_bond_price = response.data.straight_bond_price;
            val.callable_bond_price = response.data.callable_bond_price;

            val.id = uuidv4();
            let newVal = [...prevState, val];
            return newVal;
        });
    })
    .catch(error => {
        console.error('Error calculating bond price:', error);
    });
  };

  const handleDelete = id => {
    storeData('lastState', state);
    let newState = state.filter(i => {
      return i.id !== id;
    });
    setState(newState);
  };

  const handleUndo = () => {
    setState(getData('lastState'));
  };

  return (
    <div className='container'>
      <div className='row center'>
        <h1 className='white-text'> Bond Price Calculator </h1>
      </div>
      <div className='row'>
        <div className='col m12 s12'>
          <BondForm change={handleChange} />
          <div>
            <div className='row center'>
              <h4 className='white-text'> Bond Price </h4>
            </div>
            <div className='data-container row'>
              {state.length > 0 ? (
                <>
                  {state.map(info => (
                    <Info
                      key={info.id}
                      id={info.id}

                      r0={info.r0}
                      u={info.u}
                      d={info.d}
                      n={info.n}
                      q={info.q}

                      // date={info.date}
                      straight_bond_price={info.straight_bond_price}
                      callable_bond_price={info.callable_bond_price}
                      deleteCard={handleDelete}
                    />
                  ))}
                </>
              ) : (
                  <div className='center white-text'>No log found</div>
                )}
            </div>
          </div>
          {getData('lastState') !== null ? (
            <div className='center'>
              <button className='calculate-btn' onClick={handleUndo}>
                Undo
              </button>
            </div>
          ) : (
              ''
            )}
        </div>
      </div>
    </div>
  );
};

export default App;
