#ifndef ORDERING_SYSTEM_OPERATION_H
#define ORDERING_SYSTEM_OPERATION_H

#include <QtCore>
#include <QtGlobal>

namespace DianaJustEat {

	// 操作所属类型
	enum OperationKind {
		MenuOperationKind,
		MenuTypeOperationKind
	};

	// 菜品操作类型
	enum MenuOperationType {
		AddDish,
		UpdateDish,
		DeleteDish
	};

	// 菜品种类操作类型
	enum MenuTypeOperationType {
		AddType,
		UpdateType,
		DeleteType
	};

	class Operation {
	public:
		explicit Operation() = default;
		virtual OperationKind kind() = 0;
		virtual ~Operation() = default;
	};

	// 菜品操作
	class MenuOperation : public Operation {
	public:
		explicit MenuOperation() = default;
		MenuOperation(const MenuOperationType opt, const QString &dishName, const QString &dishType = "",
					  const QString &dishInfo = "", const double dishPrice = 0, const QString &dishPhoto = "", const bool dishPhotoUpdated = false);

	public:
		virtual OperationKind kind();
		MenuOperationType getOpt() const;
		QString getDishName() const;
		QString getDishType() const;
		QString getDishInfo() const;
		double getDishPrice() const;
		QString getDishPhoto() const;
		bool getDishPhotoUpdated() const;

	private:
		MenuOperationType m_opt;
		QString m_dishName;
		QString m_dishType;
		QString m_dishInfo;
		double m_dishPrice;
		QString m_dishPhoto;
		bool m_dishPhotoUpdated;
	};

	// 菜品种类操作
	class MenuTypeOperation : public Operation {
	public:
		explicit MenuTypeOperation() = default;
		MenuTypeOperation(const MenuTypeOperationType opt, const QString &typeName, const int typeNum = 0);

	public:
		virtual OperationKind kind();
		MenuTypeOperationType getOpt() const;
		QString getTypeName() const;
		int getTypeNum() const;

	private:
		MenuTypeOperationType m_opt;
		QString m_typeName;
		int m_typeNum;
	};
};// namespace DianaJustEat


#endif//ORDERING_SYSTEM_OPERATION_H
