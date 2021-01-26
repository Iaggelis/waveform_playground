#define UTILS_USE_FFTW
#include "../../pft/pft.hpp"
#include "../../pft/utils.hpp"
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TApplication.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TMath.h>
#include <TMultiGraph.h>
#include <TString.h>
#include <iostream>

using pft::Maybe;
using RDF = ROOT::RDataFrame;

std::vector<f64> savgol_filter(const std::vector<f64>& signal) {
  // calculate the coeffs
  static auto savgol_coefs = savgol_coeffs(33, 16, 16, 0, 4);

  auto filtered_signal = convln(signal, savgol_coefs);

  return filtered_signal;
}

f64 sigmoid(f64* x, f64* par) {
  return par[0] / (1.0 + TMath::Exp(-par[2] * (x[0] - par[1]))) + par[3];
}

f64 expo2(f64 *x, f64 *par) {
  auto tau1 = par[0];
  auto tau2 = par[1];
  return par[2] * TMath::Exp(-x[0] / tau2) - par[3] * TMath::Exp(-x[0] / tau1);
}

int main(int argc, char* argv[]) {
  TApplication app("", nullptr, nullptr);

  auto canvas = new TCanvas("c1", "Peak Finding");

  if (argc == 1) {
    cout << "NO FILE GIVEN\n";
    exit(1);
  }

  RDF df("t", argv[1]);

  auto df_aux = df.Define("xs",
                          [](TGraph& graph) {
                            auto points  = graph.GetX();
                            const auto n = graph.GetN();
                            return std::std::vector<f64>(points, points + n);
                          },
                          {"Waveform"})
                    .Define("ys",
                            [](TGraph& graph) {
                              auto points  = graph.GetY();
                              const auto n = graph.GetN();
                              return std::std::vector<f64>(points, points + n);
                            },
                            {"Waveform"});

  const auto xs_total    = df_aux.Take<std::std::vector<f64>>("xs").GetValue();
  const auto ys_total    = df_aux.Take<std::std::vector<f64>>("ys").GetValue();

  const auto nEvents             = ys_total.size();
  constexpr f64 MIN_PEAK_DIST = 20;

  for (size_t iEvent = 0; iEvent < nEvents; ++iEvent) {
    const auto n_of_points = ys_filtered[iEvent].size();
    const auto max_raw_ampl = static_cast<f64>(
        *max_element(ys_total[iEvent].begin(), ys_total[iEvent].end()));
    const auto min_raw_height           = 120.0;
    constMaybe<pair<f64, f64>> heights = {true, {min_raw_height, max_raw_ampl}};

    const auto peaks_indices =
        find_peaks(ys_total[iEvent], heights, MIN_PEAK_DIST);
    const auto peak_properties =
        peak_widths(ys_total[iEvent], peaks_indices, 0.90);

    const auto peak_start =
        *min_element(peak_properties.begin(), peak_properties.end(),
                     [](const auto& lhs, const auto& rhs) {
                       return lhs.left_p < rhs.left_p;
                     });
    const auto peak_end =
        *max_element(peak_properties.begin(), peak_properties.end(),
                     [](const auto& lhs, const auto& rhs) {
                       return lhs.right_p > rhs.right_p;
                     });

    // const auto main_peak = *max_element(peak_properties.begin(), peak_properties.end(), [](const auto& lhs, const auto &rhs){return lhs.}

    if (peak_start.left_p > n_of_points)
      continue;
    const auto peak_size = peak_end.right_p - peak_start.left_p;
  }
  mean_ys = pft::filter([](const auto& a) { return a != -999; }, mean_ys);
  mean_ys = pft::map([&nEvents](const auto& a) { return (a + 999) / nEvents; },
                     mean_ys);
  const auto n_points = mean_ys.size();

  auto graph_mean_wf = new TGraph(
      n_points, pft::arange<f64>(0, n_points).data(), mean_ys.data());
  graph_mean_wf->Draw();
  canvas->Draw();

  cout << "Close the program\n";
  app.Run();
  return 0;
}
