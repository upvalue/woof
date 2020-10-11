module.exports = {
  basePath: process.env.NODE_ENV === 'development' ? undefined : '/docs',
  assetPrefix: process.env.NODE_ENV === 'development' ? undefined : '/woof',
}
