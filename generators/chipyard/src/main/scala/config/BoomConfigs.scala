package chipyard

import org.chipsalliance.cde.config.{Config}

// ---------------------
// BOOM V3 Configs
// Performant, stable baseline
// ---------------------

class SmallBoomV3Config extends Config(
  new boom.v3.common.WithNSmallBooms(1) ++                          // small boom config
  new chipyard.config.AbstractConfig)

class MediumBoomV3Config extends Config(
  new boom.v3.common.WithNMediumBooms(1) ++                         // medium boom config
  new chipyard.config.AbstractConfig)

class LargeBoomV3Config extends Config(
  new boom.v3.common.WithNLargeBooms(1) ++                          // large boom config
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

class MegaBoomV3Config extends Config(
  new boom.v3.common.WithNMegaBooms(1) ++                           // mega boom config
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

class DualSmallBoomV3Config extends Config(
  new boom.v3.common.WithNSmallBooms(2) ++                          // 2 boom cores
  new chipyard.config.AbstractConfig)

class Cloned64MegaBoomV3Config extends Config(
  new boom.v3.common.WithCloneBoomTiles(63, 0) ++
  new boom.v3.common.WithNMegaBooms(1) ++                           // mega boom config
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

class LoopbackNICLargeBoomV3Config extends Config(
  new chipyard.harness.WithLoopbackNIC ++                        // drive NIC IOs with loopback
  new icenet.WithIceNIC ++                                       // build a NIC
  new boom.v3.common.WithNLargeBooms(1) ++
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

class MediumBoomV3CosimConfig extends Config(
  new chipyard.harness.WithCospike ++                            // attach spike-cosim
  new chipyard.config.WithTraceIO ++                             // enable the traceio
  new boom.v3.common.WithNMediumBooms(1) ++
  new chipyard.config.AbstractConfig)

class dmiCheckpointingMediumBoomV3Config extends Config(
  new chipyard.config.WithNPMPs(0) ++                            // remove PMPs (reduce non-core arch state)
  new chipyard.harness.WithSerialTLTiedOff ++                    // don't attach anything to serial-tl
  new chipyard.config.WithDMIDTM ++                              // have debug module expose a clocked DMI port
  new boom.v3.common.WithNMediumBooms(1) ++
  new chipyard.config.AbstractConfig)

class dmiMediumBoomV3CosimConfig extends Config(
  new chipyard.harness.WithCospike ++                            // attach spike-cosim
  new chipyard.config.WithTraceIO ++                             // enable the traceio
  new chipyard.harness.WithSerialTLTiedOff ++                    // don't attach anythint to serial-tl
  new chipyard.config.WithDMIDTM ++                              // have debug module expose a clocked DMI port
  new boom.v3.common.WithNMediumBooms(1) ++
  new chipyard.config.AbstractConfig)

class SimBlockDeviceMegaBoomV3Config extends Config(
  new chipyard.harness.WithSimBlockDevice ++                     // drive block-device IOs with SimBlockDevice
  new testchipip.iceblk.WithBlockDevice ++                       // add block-device module to peripherybus
  new boom.v3.common.WithNMegaBooms(1) ++                        // mega boom config
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

// ---------------------
// BOOM V3 Stream Buffer Prefetcher Configs
// ---------------------

// Large BOOM with default stream buffer prefetcher (4 entries, 2 lines ahead)
class LargeBoomV3WithStreamBufferConfig extends Config(
  new boom.v3.common.WithStreamBufferPrefetcher ++               // enable stream buffer prefetcher
  new boom.v3.common.WithNLargeBooms(1) ++                       // large boom config
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

// Large BOOM with aggressive stream buffer prefetcher (8 entries, 4 lines ahead)
class LargeBoomV3WithAggressiveStreamBufferConfig extends Config(
  new boom.v3.common.WithStreamBufferParams(
    nEntries = 8,
    nPrefetchAhead = 4,
    detectStride = false,
    trainThreshold = 1
  ) ++
  new boom.v3.common.WithNLargeBooms(1) ++
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

// Mega BOOM with stride-detecting stream buffer prefetcher
class MegaBoomV3WithStrideStreamBufferConfig extends Config(
  new boom.v3.common.WithStreamBufferParams(
    nEntries = 8,
    nPrefetchAhead = 4,
    detectStride = true,
    trainThreshold = 2
  ) ++
  new boom.v3.common.WithNMegaBooms(1) ++
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

// ---------------------
// BOOM V4 Configs
// Less stable and performant, but with more advanced micro-architecture
// Use for PD exploration
// ---------------------

class SmallBoomV4Config extends Config(
  new boom.v4.common.WithNSmallBooms(1) ++                          // small boom config
  new chipyard.config.AbstractConfig)

class MediumBoomV4Config extends Config(
  new boom.v4.common.WithNMediumBooms(1) ++                         // medium boom config
  new chipyard.config.AbstractConfig)

class LargeBoomV4Config extends Config(
  new boom.v4.common.WithNLargeBooms(1) ++                          // large boom config
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

class MegaBoomV4Config extends Config(
  new boom.v4.common.WithNMegaBooms(1) ++                           // mega boom config
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

class DualSmallBoomV4Config extends Config(
  new boom.v4.common.WithNSmallBooms(2) ++                          // 2 boom cores
  new chipyard.config.AbstractConfig)

class Cloned64MegaBoomV4Config extends Config(
  new boom.v4.common.WithCloneBoomTiles(63, 0) ++
  new boom.v4.common.WithNMegaBooms(1) ++                           // mega boom config
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)

class MediumBoomV4CosimConfig extends Config(
  new chipyard.harness.WithCospike ++                            // attach spike-cosim
  new chipyard.config.WithTraceIO ++                             // enable the traceio
  new boom.v4.common.WithNMediumBooms(1) ++
  new chipyard.config.AbstractConfig)

class dmiCheckpointingMediumBoomV4Config extends Config(
  new chipyard.config.WithNPMPs(0) ++                            // remove PMPs (reduce non-core arch state)
  new chipyard.harness.WithSerialTLTiedOff ++                    // don't attach anything to serial-tl
  new chipyard.config.WithDMIDTM ++                              // have debug module expose a clocked DMI port
  new boom.v4.common.WithNMediumBooms(1) ++
  new chipyard.config.AbstractConfig)

class dmiMediumBoomV4CosimConfig extends Config(
  new chipyard.harness.WithCospike ++                            // attach spike-cosim
  new chipyard.config.WithTraceIO ++                             // enable the traceio
  new chipyard.harness.WithSerialTLTiedOff ++                    // don't attach anythint to serial-tl
  new chipyard.config.WithDMIDTM ++                              // have debug module expose a clocked DMI port
  new boom.v4.common.WithNMediumBooms(1) ++
  new chipyard.config.AbstractConfig)

class SimBlockDeviceMegaBoomV4Config extends Config(
  new chipyard.harness.WithSimBlockDevice ++                     // drive block-device IOs with SimBlockDevice
  new testchipip.iceblk.WithBlockDevice ++                       // add block-device module to peripherybus
  new boom.v4.common.WithNMegaBooms(1) ++                        // mega boom config
  new chipyard.config.WithSystemBusWidth(128) ++
  new chipyard.config.AbstractConfig)
