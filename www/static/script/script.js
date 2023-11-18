window.addEventListener("DOMContentLoaded",() => {
	const c = new Clock26(".clock",true);
});

class Clock26 {
	debug = false;
	extraTime = 0;

	constructor(el,is12Hr) {
		this.el = document.querySelector(el);
		this.is12Hr = is12Hr;

		this.init();
	}
	init() {
		this.timeUpdate();

		if (this.debug === true) {
			this.el.addEventListener("click",this.addTime.bind(this,3.6e6));
		}
	}
	get timeAsObject() {
		let date = new Date();
		date = new Date(date.getTime() + this.extraTime);
		const h = date.getHours();
		const m = date.getMinutes();
		const s = date.getSeconds();

		return {h,m,s};
	}
	get timeAsString() {
		const grouped = this.timeDigitsGrouped;
		let stamp = grouped.slice(0,3).join(":").trim();

		if (this.is12Hr) stamp += ` ${grouped[3]}`;

		return stamp;
	}
	get timeDigits() {
		return this.timeDigitsGrouped.join("").split("");
	}
	get timeDigitsGrouped() {
		let {h,m,s} = this.timeAsObject;
		let ap = "";
		// prepend 0s if single digits
		if (this.is12Hr === true) {
			ap = h > 11 ? "PM" : "AM";

			if (h === 0) h = 12;
			else if (h > 12) h -= 12;
			if (h < 10) h = ` ${h}`;

		} else if (h < 10) {
			h = `0${h}`;
		}
		if (m < 10) m = `0${m}`;
		if (s < 10) s = `0${s}`;

		const groups = [h,m,s];
		if (ap) groups.push(ap);

		return groups.map(unit => `${unit}`);
	}
	addTime(amount = 0) {
		this.extraTime += amount;
		this.timeUpdate();
	}
	timeUpdate() {
		// update the `aria-label`
		this.el?.setAttribute("aria-label", this.timeAsString);
		// update the digits
		this.timeDigits.forEach((digit,i) => {
			const digitEl = this.el.querySelectorAll("[data-digit]")[i];
			if (!digitEl) return;

			digitEl.setAttribute("data-digit",digit);
		});
		// loop
		clearTimeout(this.timeUpdateLoop);
		this.timeUpdateLoop = setTimeout(this.timeUpdate.bind(this),1e3);
	}
}