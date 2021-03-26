#include "pft.hpp"

#include <ROOT/RDataFrame.hxx>
#include <TChain.h>

using RDF = ROOT::RDataFrame;

void print_usage()
{
  pft::println(stderr, "rootfileConverter usage:");
  pft::println(stderr, "./rootfileConverter [mode] [filenames.root, ...]");
  pft::println(stderr, "If [mode] = -s, save each event in a separate "
                       "file");
  pft::println(stderr, "If [mode] = -m, save all the events in a file");
}

auto main(i32 argc, c8* argv[]) -> i32
{
  if (argc <= 2) {
    print_usage();
    pft::panic("\nNO FILES GIVEN");
  }

  if (!strcmp(argv[1], "-s")) {
    Vec<pft::StringView> filenames;
    for (i32 i = 2; i < argc; ++i) {
      filenames.push_back(argv[i]);
    }

    for (const auto& filename : filenames) {
      const auto output_name =
          std::string(
              pft::split_by(pft::split_by(filename, '/').back(), '.')[0]) +
          "_converted.root";
      RDF df("t", filename);

      auto df_aux = df.Define("signal_timesteps",
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

      auto snapshot_df = df_aux.Snapshot(
          "t", output_name,
          {"channel", "channelIdx", "LightSource_time", "LightSource_ampl",
           "signal_timesteps", "signal_ampl"});
    }

  } else if (!strcmp(argv[1], "-m")) {
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

    auto snapshot_df = df_aux.Snapshot("t", "merged_converted.root",
                                       {"channel", "channelIdx",
                                        "LightSource_time", "LightSource_ampl",
                                        "signal_timesteps", "signal_ampl"});
  }

  return 0;
}
