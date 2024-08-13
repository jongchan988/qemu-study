gen = [
  decodetree.process('insn.decode', extra_args: [ '--decode', 'decode_insn',
                                                  '--varinsnwidth', '32'  ])
]

jc1904_ss = ss.source_set()
jc1904_softmmu_ss = ss.source_set()

jc1904_ss.add(gen)
jc1904_ss.add(files(
  'cpu.c',
  'disas.c',
  'translate.c'
  ))

jc1904_softmmu_ss.add(files('machine.c'))

target_arch += {'jc1904': jc1904_ss}
target_softmmu_arch += {'jc1904': jc1904_softmmu_ss}
