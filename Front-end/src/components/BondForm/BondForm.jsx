import React, { useState } from 'react';
import PropTypes from 'prop-types';
import '../App/App.css';

const initialValues = {
    r0: '',
    u: '',
    d: '',
    n: '',
    q: ''
}

const BondForm = ({ change }) => {
	const [state, setState] = useState(initialValues);

	const handleChange = e => {
		let { value, name } = e.target;
		if (value > 999) {
			value = 999;
		}
		const date = new Date().toLocaleString().split(',')[0];
		setState({
			...state,
			[name]: value,
			date
		});
	};

	const handleSubmit = () => {
		change(state);
		setState(initialValues);
	};

	return (
		<>
			<div className="row">
				<div className="center col s2">
					<label htmlFor="r0"> r0 </label>
					<input
						id="r0"
						name="r0"
						type="number"
						min="0.00"
						max="1.00"
						step="0.01"
						placeholder="0.06"
						value={state.r0}
						onChange={handleChange}
					/>
				</div>

				<div className="center col s2">
					<label htmlFor="u"> u </label>
					<input
						id="u"
						name="u"
						type="number"
						min="1.00"
						max="2.00"
						step="0.01"
						placeholder="1.20"
						value={state.u}
						onChange={handleChange}
					/>
				</div>


				<div className="center col s2">
					<label htmlFor="d">d </label>
					<input
						id="d"
						name="d"
						type="number"
						min="0.00"
						max="1.00"
						step="0.01"
						placeholder="0.90"
						value={state.d}
						onChange={handleChange}
					/>
				</div>

				<div className="center col s2">
					<label htmlFor="n">n </label>
					<input
						id="n"
						name="n"
						type="number"
						min="1"
						max="365"
						placeholder="10"
						value={state.n}
						onChange={handleChange}
					/>
				</div>

				<div className="center col s2">
					<label htmlFor="q">q </label>
					<input
						id="q"
						name="q"
						type="number"
						min="0.00"
						max="1.00"
						step="0.01"
						placeholder="0.50"
						value={state.q}
						onChange={handleChange}
					/>
				</div>

			</div>
			<div className="center">
				<button
					id="bond-btn"
					className="calculate-btn"
					type="button"
					disabled={state.r0 === '' || state.u === '' || state.d === '' || state.n === '' || state.q === ''}
					onClick={handleSubmit}
				>
					Calculate Bond Price
				</button>
			</div>
		</>
	);
};

BondForm.propTypes = {
	change: PropTypes.func.isRequired
};

export default BondForm;
