#include <QtTest/QtTest>

#include <gui/input.h>

class TestInputMac : public QObject
{
	Q_OBJECT

private slots:
	void AltSpecialCharacters() noexcept;
	void LessThanModifierKeys() noexcept;
	void SpecialKeys() noexcept;
	void KeyboardLayoutUnicodeHexInput() noexcept;
};

void TestInputMac::AltSpecialCharacters() noexcept
{
	// Issue#510: MacOS Alt special key input does not work.
	QKeyEvent evAltA{ QEvent::KeyPress, Qt::Key_A, Qt::AltModifier, "å" };
	QCOMPARE(NeovimQt::Input::convertKey(evAltA), QString{ "å" });

	QKeyEvent evShiftAltA{ QEvent::KeyPress, Qt::Key_A, Qt::ShiftModifier | Qt::AltModifier, "Å" };
	QCOMPARE(NeovimQt::Input::convertKey(evShiftAltA), QString{ "Å" });

	QKeyEvent evShiftAltL{ QEvent::KeyPress, Qt::Key_L, Qt::ShiftModifier | Qt::AltModifier, "Ò" };
	QCOMPARE(NeovimQt::Input::convertKey(evShiftAltL), QString{ "Ò" });
}

void TestInputMac::LessThanModifierKeys() noexcept
{
	QKeyEvent evLessThanControl{ QEvent::KeyPress, Qt::Key::Key_Less, Qt::ShiftModifier | Qt::ControlModifier, "<" };
	QCOMPARE(NeovimQt::Input::convertKey(evLessThanControl), QString{ "<D-lt>" });

	QKeyEvent evLessThanAlt{ QEvent::KeyPress, Qt::Key::Key_Less, Qt::ShiftModifier | Qt::AltModifier, "<" };
	QCOMPARE(NeovimQt::Input::convertKey(evLessThanAlt), QString{ "<A-lt>" });

	QKeyEvent evLessThanMeta{ QEvent::KeyPress, Qt::Key::Key_Less, Qt::ShiftModifier | Qt::MetaModifier, "<" };
	QCOMPARE(NeovimQt::Input::convertKey(evLessThanMeta), QString{ "<C-lt>" });
}

void TestInputMac::SpecialKeys() noexcept
{
	const QMap<int, QString>& specialKeys { NeovimQt::Input::GetSpecialKeysMap() };

	for (const auto k : specialKeys.keys()) {
		// On Mac Meta is the Control key, treated as C-.
		QList<QPair<QKeyEvent, QString>> keyEventList{
			{ { QEvent::KeyPress, k, Qt::NoModifier, {} },      "<%1>" },
			{ { QEvent::KeyPress, k, Qt::ControlModifier, {} }, "<D-%1>" },
			{ { QEvent::KeyPress, k, Qt::AltModifier, {} },     "<A-%1>" },
			{ { QEvent::KeyPress, k, Qt::MetaModifier, {} },    "<C-%1>" },
		};

		for (const auto& keyEvent : keyEventList) {
			QCOMPARE(NeovimQt::Input::convertKey(keyEvent.first),
				keyEvent.second.arg(specialKeys.value(k)));
		}
	}
}

void TestInputMac::KeyboardLayoutUnicodeHexInput() noexcept
{
	// Issue#579: Cannot map <A-...> on MacOS
	QKeyEvent evAltA{ QEvent::KeyPress, Qt::Key_A, Qt::AltModifier };
	QCOMPARE(NeovimQt::Input::convertKey(evAltA), QString{ "<A-a>" });

	QKeyEvent evAltShiftA{ QEvent::KeyPress, Qt::Key_A, Qt::AltModifier | Qt::ShiftModifier };
	QCOMPARE(NeovimQt::Input::convertKey(evAltShiftA), QString{ "<A-A>" });

	QKeyEvent evCtrlAltA{ QEvent::KeyPress, Qt::Key_A, Qt::MetaModifier | Qt::AltModifier };
	QCOMPARE(NeovimQt::Input::convertKey(evCtrlAltA), QString{ "<C-A-a>" });

	QKeyEvent evCtrlAltShiftA{ QEvent::KeyPress, Qt::Key_A,
		Qt::MetaModifier | Qt::AltModifier | Qt::ShiftModifier };
	QCOMPARE(NeovimQt::Input::convertKey(evCtrlAltShiftA), QString{ "<C-A-A>" });
}

#include "tst_input_mac.moc"
QTEST_MAIN(TestInputMac)
