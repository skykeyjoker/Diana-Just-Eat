#include "TcpHeart.h"

TcpHeart::TcpHeart(QObject *parent)
	: QObject(parent) {
	m_heart_timer = new QTimer(this);
	m_count = 0;

	connect(m_heart_timer, &QTimer::timeout, this, &TcpHeart::slotTimeOut);
}

TcpHeart::~TcpHeart() noexcept {
}

void TcpHeart::startHeartTimer() {
	m_heart_timer->start(2000);
}

void TcpHeart::slotTimeOut() {
	if (m_count > 2) {
		m_count = 0;
		m_heart_timer->stop();
		emit sigHeartBad();
		return;
	}
	m_count++;
	emit sigHeartReq();
}

void TcpHeart::slotHeartBack() {
	m_count = 0;
}