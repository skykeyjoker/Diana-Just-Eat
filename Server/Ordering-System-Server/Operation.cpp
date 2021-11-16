#include "Operation.h"

namespace DianaJustEat {
	DianaJustEat::MenuOperation::MenuOperation(const DianaJustEat::MenuOperationType opt, const QString& dishName, const QString& dishType,
											   const QString& dishInfo, const double dishPrice, const QString& dishPhoto, const bool dishPhotoUpdated)
		: Operation(),
		  m_opt(opt),
		  m_dishName(dishName),
		  m_dishType(dishType),
		  m_dishInfo(dishInfo),
		  m_dishPrice(dishPrice),
		  m_dishPhoto(dishPhoto),
		  m_dishPhotoUpdated(dishPhotoUpdated) {
	}

	DianaJustEat::OperationKind DianaJustEat::MenuOperation::kind() {
		return OperationKind::MenuOperationKind;
	}

	DianaJustEat::MenuOperationType DianaJustEat::MenuOperation::getOpt() const {
		return m_opt;
	}

	QString DianaJustEat::MenuOperation::getDishName() const {
		return m_dishName;
	}

	QString DianaJustEat::MenuOperation::getDishType() const {
		return m_dishType;
	}

	QString DianaJustEat::MenuOperation::getDishInfo() const {
		return m_dishInfo;
	}

	double DianaJustEat::MenuOperation::getDishPrice() const {
		return m_dishPrice;
	}

	QString DianaJustEat::MenuOperation::getDishPhoto() const {
		return m_dishPhoto;
	}

	bool DianaJustEat::MenuOperation::getDishPhotoUpdated() const {
		return m_dishPhotoUpdated;
	}

	MenuTypeOperation::MenuTypeOperation(const MenuTypeOperationType opt, const QString& typeName, const int typeNum)
		: m_opt(opt),
		  m_typeName(typeName),
		  m_typeNum(typeNum) {
	}

	OperationKind MenuTypeOperation::kind() {
		return OperationKind::MenuTypeOperationKind;
	}

	MenuTypeOperationType MenuTypeOperation::getOpt() const {
		return m_opt;
	}

	QString MenuTypeOperation::getTypeName() const {
		return m_typeName;
	}

	int MenuTypeOperation::getTypeNum() const {
		return m_typeNum;
	}
};// namespace DianaJustEat