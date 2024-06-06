import React from 'react';
import PropTypes from 'prop-types';

const Info = ({ straight_bond_price, callable_bond_price, r0, u, d, n, q, id, deleteCard }) => {
  const handleDelete = () => {
    deleteCard(id);
  };

  return (
    <div className="col m6 s12">
      <div className="card">
        <div className="card-content">
          <span className="card-title" data-test="straight_bond_price">
            Straight bond price: {straight_bond_price}
          </span>
          <span className="card-title" data-test="callable_bond_price">
          Callable bond price: {callable_bond_price}
          </span>
          <div className="card-data">
            <span data-test="r0">r0: {r0} </span>
            <span data-test="u">u: {u} </span>
            <span data-test="d">d: {d} </span>
            <span data-test="n">n: {n} </span>
            <span data-test="q">q: {q} </span>

          </div>

          <button className="delete-btn" onClick={handleDelete}>
            X
          </button>
        </div>
      </div>
    </div>
  );
};

Info.propTypes = {
  r0: PropTypes.string,
  u: PropTypes.string,
  d: PropTypes.string,
  n: PropTypes.string,
  q: PropTypes.string,

  straight_bond_price: PropTypes.string,
  callable_bond_price: PropTypes.string
};

export default Info;
