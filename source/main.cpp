#include <numeric>
#include "LabSource.h"
#include "ConsoleGraph.h"

int main() {

    HWND hWnd = GetForegroundWindow();
    ShowWindow(hWnd, SW_SHOWMAXIMIZED);

    /* ��������� ���������� */
    double carrier = 13e6;
    double period = 1 / carrier;
    double symbol = 3 * period;
    double timePoint1 = 0;
    double timePoint2 = 8 * symbol;

    /* ������� ������ */
    LabSource src(timePoint1, timePoint2, carrier);
    auto modulation = getCodeMod(timePoint2 / 8, { 0.75, 0, 0.5, 0.25, 1, 1.25, 1.75, 1.5 });
    src.setModParameters({ modulation, Modulation::PH, 0.5 });
    src.sin();

    /* ������� ������ */
    LabSource sup(timePoint1, timePoint2, carrier);
    sup.sin().hilbertTransform();

    /* ������������ �������� ������� � ��������� � ��������� �� pi/2 ������� */
    auto receive = src * sup;

    /* ���������� */
    DMath correlation = receive;
    int pointsPerSymbol = symbol * carrier * 64;
    for (int i = 0; i < receive.function.size(); i += pointsPerSymbol) {
        auto begin = correlation.function.begin() + i;
        auto end = correlation.function.begin() + i + pointsPerSymbol;
        std::partial_sum(begin, end, begin);
    }

    /* ���������� ��������� */
    DMath star;
    for (int i = pointsPerSymbol - 1; i < receive.function.size(); i += pointsPerSymbol) {
        star.function.push_back(correlation.function[i].real()/pointsPerSymbol);
        star.argument.push_back(correlation.function[i].imag()/ pointsPerSymbol);
    }

    /* ���������� �������� */
    ConsoleGraph graph(750, 950, {6, 2});
    graph.plot(src.getValue());                                     // ������� ������
    graph.plot(src.cmFT().getValue(Format::ABS), 1, 2);             // ������ �������� �������
    graph.plot(receive.getValue(), 2, 1);                           // ��������� ������������ � ������� (Real)
    graph.plot(receive.getValue(Format::IMAG), 2, 2);               // ��������� ������������ � ������� (Imag)
    graph.plot(correlation.getValue(), 3, 1);                       // ����� ����������� (Real)    
    graph.plot(correlation.getValue(Format::IMAG), 3, 2);           // ����� ����������� (Imag)
    graph.plot(correlation.getValue(Format::ABS), 4, 1, 1, 2);      // ����� ����������� (Abs)

    graph.Type = "Dot";
    graph.Scale = "Even";
    graph.plot(star.getValue(), 5, 1, 2, 2);                        // ���������


    return 0;
}