#ifndef ORDERING_SYSTEM_TCPHEART_H
#define ORDERING_SYSTEM_TCPHEART_H

#include <QObject>
#include <QTimer>

class TcpHeart : public QObject {
	Q_OBJECT
public:
	explicit TcpHeart(QObject* parent = 0);
	~TcpHeart() noexcept;

public:
	void startHeartTimer();

signals:
	void sigHeartBad();
	void sigHeartReq();

private slots:
	void slotTimeOut();

public slots:
	void slotHeartBack();

private:
	QTimer* m_heart_timer;
	int m_count;
};


#endif//ORDERING_SYSTEM_TCPHEART_H
