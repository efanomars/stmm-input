/*
 * Copyright © 2016  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   callifsimplifier.cc
 */

#include "callifsimplifier.h"

namespace stmi
{

namespace CallIfSimplifier
{

const shared_ptr<CallIf> simplify(
						const shared_ptr<CallIf>& refCallIf, const Event::Class& oEventClass)
{
//std::cout << "CallIfSimplifier::simplify  oEventClass=" << oEventClass.getId() << std::endl;
	const std::type_info& oCallIfType = typeid(*refCallIf);
	if (oCallIfType == typeid(CallIfEventClass)) {
		auto refECF = std::static_pointer_cast<CallIfEventClass>(refCallIf);
		if (refECF->getClass() == oEventClass) {
			return CallIfTrue::getInstance(); //--------------------------------
		} else {
			return CallIfFalse::getInstance(); //-------------------------------
		}
	} else if (oCallIfType == typeid(CallIfXYEvent)) {
		if (oEventClass.isXYEvent()) {
			return CallIfTrue::getInstance(); //--------------------------------
		} else {
			return CallIfFalse::getInstance(); //-------------------------------
		}
	} else if (oCallIfType == typeid(CallIfAnd)) {
		auto refAndF = std::static_pointer_cast<CallIfAnd>(refCallIf);
		auto refSF1 = simplify(refAndF->getCallIf1(), oEventClass);
		auto refSF2 = simplify(refAndF->getCallIf2(), oEventClass);
		const std::type_info& oSF1Type = typeid(*refSF1);
		const std::type_info& oSF2Type = typeid(*refSF2);
		if (oSF1Type == typeid(CallIfTrue)) {
			// And(True,Op2) = Op2
			return refSF2; //---------------------------------------------------
		} else if (oSF2Type == typeid(CallIfTrue)) {
			// And(Op1,True) = Op1
			return refSF1; //---------------------------------------------------
		} else if ((oSF1Type == typeid(CallIfFalse)) || (oSF2Type == typeid(CallIfFalse))) {
			// And(False,Op2) = False,  And(Op1,False) = False
			return CallIfFalse::getInstance(); //-------------------------------
		}
	} else if (oCallIfType == typeid(CallIfOr)) {
		auto refOrF = std::static_pointer_cast<CallIfOr>(refCallIf);
		auto refSF1 = simplify(refOrF->getCallIf1(), oEventClass);
		auto refSF2 = simplify(refOrF->getCallIf2(), oEventClass);
		const std::type_info& oSF1Type = typeid(*refSF1);
		const std::type_info& oSF2Type = typeid(*refSF2);
		if (oSF1Type == typeid(CallIfFalse)) {
			// Or(False, Op2) = Op2
			return refSF2; //---------------------------------------------------
		} else if (oSF2Type == typeid(CallIfFalse)) {
			// Or(Op1,False) = Op1
			return refSF1; //---------------------------------------------------
		} else if ((oSF1Type == typeid(CallIfTrue)) || (oSF2Type == typeid(CallIfTrue))) {
			// Or(True, Op2) = True,  Or(Op1, True) = True
			return CallIfTrue::getInstance(); //--------------------------------
		}
	} else if (oCallIfType == typeid(CallIfNot)) {
		auto refNotF = std::static_pointer_cast<CallIfNot>(refCallIf);
		auto refSF = simplify(refNotF->getCallIf(), oEventClass);
		const std::type_info& oSFType = typeid(*refSF);
		if (oSFType == typeid(CallIfFalse)) {
			// Not(False) = True
			return CallIfTrue::getInstance(); //--------------------------------
		} else if (oSFType == typeid(CallIfTrue)) {
			// Not(True) = False
			return CallIfFalse::getInstance(); //-------------------------------
		} else if (oSFType == typeid(CallIfNot)) {
			// Not(Not(Op)) = Op
			auto refNNF = std::static_pointer_cast<CallIfNot>(refSF);
			return refNNF->getCallIf(); //--------------------------------------
		}
	}
	// cannot simplify
	return refCallIf;
}

} // namespace CallIfSimplifier

} // namespace stmi
