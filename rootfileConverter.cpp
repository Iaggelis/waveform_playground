#include "pft.hpp"

#include <ROOT/RDataFrame.hxx>
#include <TChain.h>
#include <iostream>

using RDF = ROOT::RDataFrame;

auto main(i32 argc, c8* argv[]) -> i32
{

  if (argc == 1) {
    pft::panic("NO FILES GIVEN");
  }

  TChain chain("t");
  for (i32 i = 1; i < argc; ++i) {
    chain.Add(argv[i]);
  }

  RDF df_from_chain(chain);

  auto df_aux = df_from_chain
                    .Define("signal_timesteps",
                            [](const TGraph& graph) {
                              const auto points = graph.GetX();
                              const auto n      = graph.GetN();
                              return Vec<f64>(points, points + n);
                            },
                            {"Waveform"})
                    .Define("signal_ampl",
                            [](const TGraph& graph) {
                              const auto points = graph.GetY();
                              const auto n      = graph.GetN();
                              return Vec<f64>(points, points + n);
                            },
                            {"Waveform"});

  auto snapshot_df =
      df_aux.Snapshot("t", "converted.root",
                      {"channel", "channelIdx", "LightSource_time",
                       "LightSource_ampl", "signal_timesteps", "signal_ampl"});
  return 0;
}
